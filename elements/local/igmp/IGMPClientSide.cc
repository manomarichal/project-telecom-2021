#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include "IGMPClientSide.hh"

CLICK_DECLS
IGMPClientSide::IGMPClientSide()
{}

IGMPClientSide::~IGMPClientSide()
{}

int IGMPClientSide::configure(Vector<String> &conf, ErrorHandler *errh)
{
    if (Args(conf, this, errh).read_m("MAXPACKETSIZE", maxSize).complete() < 0) return -1;
    if (maxSize <= 0) return errh->error("maxsize should be larger than 0");
    return 0;}

/*
 * handles the client join for the system.
 * return integer, done throughout other click elements
 */
int IGMPClientSide::client_join(const String &conf, Element *e, void *thunk, ErrorHandler *errh) {
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    Vector<String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector
    //make new group record if it does not yet exist
    //    igmp_group_record grRecord;


//    IPAddress groupaddr = IPAddress(arg_list[0]);
//    bool exists = false;
//    for(int i = 0; i<= group_records.size(); i++){
//        if (group_records[i].multicast_adress == groupaddr){
//            //check if the client is a part of this group record already
//            for (int y =0; y<=group_records[i].source_adresses.size(); y++){
//                if (group_records[i].source_adresses[y] == clientIP){
//                    //case if the client has already joined the group address
//                    //the mc address exists and the client is already a part of it
//                    return -1;
//                }
//            }
//            //if the client is not a part off the group, add it to the group
//            group_records[i].source_adresses.push_back(clientIP);
//            group_records[i].number_of_sources++;
//            exists = true;
//        }
//    }
//    if(!exists){
//        //the group record needs to be made
//        grRecord.record_type = 0;
//        grRecord.multicast_adress = groupaddr;
//        grRecord.number_of_sources = 1;
//        grRecord.source_adresses.push_back(clientIP);
//    }


    click_chatter("hallo, join");
    return 0;
}

int IGMPClientSide::client_leave(const String &conf, Element *e, void *thunk, ErrorHandler *errh) {
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    Vector<String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector



    click_chatter("hallo, leave");
    return 0;
}

/*
 * function which initialises the handlers, current handlers contain:
 * -client_join which will let the client join the reception of a given multicast address /rfc page 4
 * -client leave which wil let the client leave the reception of a given multicast address /rfc page 4
 */
void IGMPClientSide::add_handlers() {
    click_chatter("hallo, adding handlers");

    add_write_handler("client_join", &client_join, 0);
    add_write_handler("client_leave", &client_leave, 0);
}

// makes membership v3 packets, based on RFC3376 page 13-14
Packet * IGMPClientSide::make_membership_packet()
{
    uint32_t size = 1; // idk yet how to calculate this
    WritablePacket *p = Packet::make(size);
    memset(p->data(), '\0', p->length()); // sets all the data to \0's, ¯\_(ツ)_/¯ but examples do it

    // IP HEADER, based on elements/icmp/icmpsendpings.cc
    click_ip *nip = reinterpret_cast<click_ip *>(p->data()); // why reinterpret, ¯\_(ツ)_/¯
    nip->ip_v = 4;
    nip->ip_hl = (sizeof(click_ip)) >> 2; // IHL field, right shift by 2, ¯\_(ツ)_/¯
    nip->ip_len = htons(p->length()); // converts host byte order to network byte order
    uint16_t ip_id = 1; // TODO no idea
    nip->ip_id = htons(ip_id); // converts host byte order to network byte order
    nip->ip_p = IP_PROTO_IGMP; //TODO found in elements/ip/ipnameinfo.cc, either this or 2 (according to RFC3367 page 7)
    nip->ip_ttl = 1; // specified in RFC3376 page 7
    nip->ip_src = IPAddress(); // TODO no variable for it yet
    nip->ip_dst = IPAddress(("224.0.0.1")); // all multicast routers listen to this adress
    nip->ip_sum = click_in_cksum((unsigned char *)nip, sizeof(click_ip)); // TODO no idea

    // IP OPTIONS, every packet needs an IP Router Alert option [RFC-2113], specified in RFC3376 page 7
    // TODO

    // IGMP membership reports


}
CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

