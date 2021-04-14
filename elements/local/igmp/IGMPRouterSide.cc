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

void IGMPRouterSide::multicast_packet(Packet *p)
{
    const click_ip *ip_header = p->ip_header();

    for (igmp_group_state group: group_states)
    {
        if (group.multicast_adress == ip_header->ip_dst)
        {
            click_chatter("MULTICASTING FROM %s", IPAddress(group.multicast_adress).unparse().c_str());
        }
    }
}

void IGMPRouterSide::update_group_states(Vector<igmp_group_record> group_records)
{
    for (igmp_group_record record:group_records)
    {
        // should use hashmap here, but click hashmaps are jank, maybe later as optimization
        for (igmp_group_state state:group_states)
        {
            if (record.multicast_adress == state.multicast_adress)
            {
                if (record.record_type == IGMP_V3_CHANGE_TO_INCLUDE) {state.mode = IGMP_V3_INCLUDE;}
                if (record.record_type == IGMP_V3_CHANGE_TO_EXCLUDE) {state.mode = IGMP_V3_EXCLUDE;}
            }
        }
        // no group state exists, make new one
        igmp_group_state new_state;
        new_state.mode = record.record_type;
        new_state.multicast_adress = record.multicast_adress;
        new_state.clients = record.sources;
    }
}

void IGMPRouterSide::push(int port, Packet *p){

    const click_ip *ip_header = p->ip_header();

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
        click_chatter("IGMP PACKET, %i, %i", ip_header->ip_p, port);
        // unpacking data
        igmp_mem_report report_info = helper->igmp_unpack_info(p->data());
        Vector<igmp_group_record> group_records = helper->igmp_unpack_group_records(reinterpret_cast<const igmp_mem_report*>(p->data()) + 1, report_info.number_of_group_records);
        update_group_states(group_records);
    }
    else if(ip_header->ip_p == 80)
    {
        click_chatter("UDP PACKET, %i, %i", ip_header->ip_p, port);
        multicast_packet(p);
    }
    else
    {
        click_chatter("UKNOWN PACKET, %i, %i", ip_header->ip_p, port);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterSide) // forces to create element within click
