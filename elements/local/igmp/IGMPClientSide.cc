#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include "IGMPClientSide.hh"
#include "./packethelpers.h"
CLICK_DECLS
IGMPClientSide::IGMPClientSide()
{}

IGMPClientSide::~IGMPClientSide()
{}

/*
 * configure for clientside object, should set the clientIP to the given clientaddress
 * might need to add some things later like timer variables
 * CLIENTADDRESS
 * MULTICASTADDRESS
 * ALLSYSTEMSMULTICASTADDRESS
 */
int IGMPClientSide::configure(Vector<String> &conf, ErrorHandler *errh)
{
    String ipadresstest;
    String madr;
    String asmadr;
    if (Args(conf, this, errh).read_mp("CADDR", clientIP).read_mp("MADDR", MC_ADDRESS).read_mp("ASMADDR",ALL_SYSTEMS_MC_ADDRESS).complete()<0){click_chatter("failed read, returning 0", ipadresstest); return -1;}
    click_chatter("initialising ipaddress %s", clientIP.unparse().c_str());
    click_chatter("initialising mc address %s", MC_ADDRESS.unparse().c_str());
    click_chatter("initialising asmc address %s", ALL_SYSTEMS_MC_ADDRESS.unparse().c_str());
    return 0;
}

/*
 * handles the client join for the system.
 * return integer, done throughout other click elements
 *
 */
int IGMPClientSide::client_join(const String &conf, Element *e, __attribute__((unused)) void *thunk, __attribute__((unused)) ErrorHandler *errh) {
    click_chatter("Entering join handler");
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    click_chatter(element->clientIP.unparse().c_str());
    Vector<String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector

    uint32_t groupaddr = IPAddress(arg_list[0]).addr();

    bool exists = false;
    for(int i = 0; i< element->group_records.size(); i++){
        if (element->group_records[i].multicast_adress == groupaddr){
            //check if the client is a part of this group record already
//            for (int y =0; y<element->group_records[i].sources.size(); y++){
//                if (element->group_records[i].sources[y] == element->clientIP){
//                    //case if the client has already joined the group address
//                    //the mc address exists and the client is already a part of it
//                    click_chatter("The group you are trying to join already has this source address");
//                    return -1;
//                }
//            }

            //if the client is not a part off the group, add it to the group
//            element->group_records[i].sources.push_back(element->clientIP);
//            element->group_records[i].number_of_sources++;

            click_chatter("You have already joined this multicast group");
            return -1;
        }
    }

        //make new group record if it does not yet exist
    igmp_group_record grRecord;
    //the group record needs to be made
    grRecord.record_type = change_to_exclude;
    grRecord.multicast_adress = groupaddr;
    grRecord.number_of_sources = 0;
//        grRecord.sources.push_back(element->clientIP);
    element->group_records.push_back(grRecord);
    click_chatter("while completing join, the mode is now %d", element->group_records[0].record_type);

    click_chatter("making a new group record");


//        grRecord.print_record();

    WritablePacket * p =element->make_mem_report_packet();
    element->output(0).push(p);
    click_chatter("completed join");
    element->print_group_records();
    return 0;
}
/*
 * handles the client leave, changes the filter mode fo that group record from exclude to include as defined in RFC
 *
 */
int IGMPClientSide::client_leave(const String &conf, Element *e, __attribute__((unused)) void *thunk, __attribute__((unused)) ErrorHandler *errh) {
    click_chatter("Entering leave handler");
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    Vector<String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector
    uint32_t groupaddr = IPAddress(arg_list[0]).addr();
    //iterate over the different group records
    bool found_group = false;
    for(int i = 0; i< element->group_records.size(); i++){
        //if the grouprecord has the same address as the input address then go into statement
        if (element->group_records[i].multicast_adress == groupaddr) {
            //The group address exists
            found_group = true;
            click_chatter("while completing leave, the mode is now %d", element->group_records[i].record_type);
            element->group_records[i].record_type = change_to_include;
            click_chatter("while completing leave, the mode is after the change %d", element->group_records[i].record_type);

            WritablePacket * p =element->make_mem_report_packet();
            //push the packet to update mode
            element->output(0).push(p);
            click_chatter("completed leave");
//            bool found_client = false;
//            for (int y =0; y<element->group_records[i].sources.size(); y++){
//                if (element->group_records[i].sources[y] == element->clientIP){
//                    //the client's address has been found within the group record's source addresses
//                    found_client = true;
//                    element->group_records[i].mode = change_to_include;
//                    WritablePacket * p =element->make_mem_report_packet();
//                    //push the packet to update mode
//                    element->output(0).push(p);
//                    click_chatter("completed leave");
//                }
//            }
//            if(!found_client){
//                //the client does not exist in the group record
//                click_chatter("did not find client when executing leave");
//                return -1;
//            }

        }
    }
    if(!found_group){
        //the group has not been found
        click_chatter("did not find group when executing leave");
        return -1;
    }
    return 0;
}

/*
 * function which initialises the handlers, current handlers contain:
 * -client_join which will let the client join the reception of a given multicast address /rfc page 4
 * -client leave which wil let the client leave the reception of a given multicast address /rfc page 4
 */
void IGMPClientSide::add_handlers() {
    click_chatter("hallo, adding handlers");

    add_write_handler("client_join", &client_join, (void*)0);
    add_write_handler("client_leave", &client_leave, (void*)0);
}

void IGMPClientSide::print_group_records(){
    for(int i = 0; i< group_records.size(); i++){
        click_chatter("group_record %i", i);
        click_chatter("\t multicast_adress %s", group_records[i].multicast_adress.unparse().c_str());
        click_chatter("\t record_type %d", group_records[i].record_type);
        click_chatter("\t number_of_sources %i", group_records[i].number_of_sources);
        click_chatter("\t mode %d", (int)group_records[i].record_type);
        for(int y =0; y<group_records[i].sources.size(); y++){
            click_chatter("\t source %i %s",y, group_records[i].sources[y].unparse().c_str());
        }
    }
}


////////////////////////
/// PACKET FUNCTIONS ///
////////////////////////

// makes membership v3 packets, based on RFC3376 page 13-14
WritablePacket * IGMPClientSide::make_mem_report_packet()
{
    click_chatter("creating membership report packed");

    //uint32_t size = sizeof(click_ip) + sizeof(igmp_mem_report) + (sizeof(igmp_group_record_message)*group_records.size()); // TODO size of the entire packet
    WritablePacket *p = Packet::make(igmp_v3_reports::get_size_of_data(group_records) + sizeof(click_ip) + 4);
    memset(p->data(), 0, p->length()); // erase previous random data on memory requested

    click_ip *ip_header = igmp_v3_reports::add_ip_header(p, clientIP, MC_ADDRESS, true);
    router_alert* r_alert = igmp_v3_reports::add_router_alert(ip_header+1);
    igmp_v3_reports::add_igmp_data(r_alert+1, group_records);

    p->set_ip_header(ip_header, sizeof(click_ip));
    p->timestamp_anno().assign_now();
    p->set_dst_ip_anno(IPAddress(MC_ADDRESS));

    click_chatter("\t igmp data size: %i", igmp_v3_reports::get_size_of_data(group_records));
    click_chatter("\t size of ip header: %i", sizeof(click_ip) + 4);
    click_chatter("packet finished");
    return p;
}

void IGMPClientSide::push(int port, Packet *p)
{
    // TODO needs to accept and process queries (also something about udp)
    // unpacking data, based on elements/icmp/icmpsendpings.cc, line 194
    click_chatter("IGMP CLIENT %s recieved a packet in port %i ", clientIP.unparse().c_str(), port);
    const click_ip *iph = p->ip_header();

    // IGMP QUERIES
    if (port == 0)
    {
        // TODO
        return;
    }
    // UDP MESSAGES
    else if (port == 1)
    {
        if (p->has_network_header() and iph->ip_dst == clientIP)
        {
            output(1).push(p);
        }
        else
        {
            p->kill();
        }
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide) // forces to create element within click


