//
// Created by Gebruiker on 7/04/2021.
//
#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include <clicknet/ip.h>
#include "IGMPRouterSide.hh"

CLICK_DECLS
IGMPRouterSide::IGMPRouterSide()
{}

IGMPRouterSide::~IGMPRouterSide()
{}

/*
 * configure for routerside object, only takes therouterip or interface as input
 */
int IGMPRouterSide::configure(Vector<String> &conf, ErrorHandler *errh)
{
    if (Args(conf, this, errh).read_mp("ROUTERADDRESS", routerIP).complete()<0){click_chatter("failed read when initialising router, returning 0"); return -1;}
    click_chatter("initialising routeraddress %s", routerIP.unparse().c_str());
    return 0;
}

void IGMPRouterSide::multicast_packet(Packet *p, int port)
{
    const click_ip *ip_header = p->ip_header();

    for (igmp_group_state group: group_states) {
        if (group.multicast_adress == ip_header->ip_dst)
        {
            int ports = (int)port_count()[2] - 48;
            click_chatter("multicasting to %i ports", ports);

            for (int i = 0; i < ports; i++)
            {
                output(i).push(p);
            }
        }
    }

    /*
    for (igmp_group_state group: group_states)
    {
        //click_chatter("checking %s vs %s ", IPAddress(group.multicast_adress).unparse().c_str(), IPAddress(ip_header->ip_dst).unparse().c_str());
        if (group.multicast_adress == ip_header->ip_dst)
        {
            click_chatter("multicasting in group %s with %i members", IPAddress(group.multicast_adress).unparse().c_str(), group.clients.size());
            for (IPAddress client: group.clients)
            {
                click_chatter("\tmulticasted to %s", client.unparse().c_str());
                WritablePacket *new_packet = p->uniqueify();
                click_ip *new_ip_header = new_packet->ip_header();
                new_ip_header->ip_src = group.multicast_adress.in_addr();
                new_ip_header->ip_dst = client.in_addr();
                new_ip_header->ip_sum = click_in_cksum((unsigned char*) new_ip_header, sizeof(click_ip));
                new_packet->set_ip_header(new_ip_header, sizeof(click_ip));
                new_packet->timestamp_anno().assign_now();
                new_packet->set_dst_ip_anno(IPAddress(client));
                output(port).push(new_packet);
            }
        }
    }
     */
}

void IGMPRouterSide::update_group_state(const click_ip *ip_header, igmp_group_state state, igmp_group_record record)
{
    // add client to group state if it is not in it
    //click_chatter("updating groupstate %s", state.multicast_adress.unparse().c_str());

    /*
    bool already_in = false;
    for (IPAddress client: state.clients)
    {
        if (client == ip_header->ip_src)
        {
            already_in = true;
        }
    }
    if (!already_in)
    {
        click_chatter("adding %s to group %s", IPAddress(ip_header->ip_src).unparse().c_str(), state.multicast_adress.unparse().c_str());
        state.clients.push_back(IPAddress(ip_header->ip_src));
    }
     */

    /* idk what to do here yet
    // update client mode
    if (record.record_type == IGMP_V3_CHANGE_TO_INCLUDE)
    {
        state.mode = IGMP_V3_INCLUDE;
    }
    else if (record.record_type == IGMP_V3_CHANGE_TO_EXCLUDE)
    {
        state.mode = IGMP_V3_EXCLUDE;
    }
     */
}

void IGMPRouterSide::update_group_states(const click_ip *ip_header, Vector<igmp_group_record> group_records)
{
    bool VERBOSE = true;

    for (igmp_group_record record:group_records)
    {
        bool exists = false;
        // should use hashmap here, but click hashmaps are jank, maybe later as optimization
        for (igmp_group_state state:group_states)
        {
            if (record.multicast_adress == state.multicast_adress)
            {
                exists = true;
                update_group_state(ip_header, state, record);
            }
        }

        // if no group state exists for the multicast adress, make new one
        if (!exists)
        {
            igmp_group_state new_state;
            new_state.mode = record.record_type;
            new_state.multicast_adress = record.multicast_adress;
            // new_state.clients.push_back(ip_header->ip_src);
            group_states.push_back(new_state);
            if (VERBOSE)
            {
                click_chatter("CREATING NEW GROUP STATE");
                click_chatter("\t multicast adress: %s", IPAddress(record.multicast_adress).unparse().c_str());
                // click_chatter("\t client adress: %s", IPAddress(ip_header->ip_src).unparse().c_str());
                click_chatter("\t mode: %i", record.record_type);
            }
        }

    }
}

void IGMPRouterSide::push(int port, Packet *p){

    const click_ip *ip_header = p->ip_header();
    click_chatter("PACKET, %i, %i", ip_header->ip_p, port);

    if(ip_header->ip_p == IP_PROTO_IGMP){
        /*
         * input port of the igmp messages
         * ontbind de igmp header
         * kijk na of het een v3 membership report is
         * haal de filter mode uit de igmp header
         * als filtermode exclude is (en de src list leeg is) voer je join uit
         *      voeg toe aan group state, zet timer
         * als filtermode include is (en de src list is leeg) voer je leave uit
         *      voeg toe aan group state, zet timer
         * steeds kijken of deze messages geen repeat messages zijn, deze renewed de timers enkel en moet dus niet opnieuw worden geadd
         */
        //click_chatter("IGMP PACKET type %i, port %i", ip_header->ip_p, port);
        // unpacking data
        const router_alert* alert_ptr = reinterpret_cast<const router_alert*>(ip_header+1);
        const igmp_mem_report* info_ptr = reinterpret_cast<const igmp_mem_report*>(alert_ptr+1);
        const igmp_group_record_message *records_ptr = reinterpret_cast<const igmp_group_record_message*>(info_ptr+1);

        igmp_mem_report report_info = helper->igmp_unpack_info(info_ptr);
        Vector<igmp_group_record> group_records = helper->igmp_unpack_group_records(records_ptr, report_info.number_of_group_records);
        update_group_states(ip_header, group_records);
    }
    else if(ip_header->ip_p == 17)
    {
        //click_chatter("UDP PACKET, %i, %i", ip_header->ip_p, port);
        multicast_packet(p, port);
    }
    else
    {
        output(port).push(p);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterSide) // forces to create element within click
