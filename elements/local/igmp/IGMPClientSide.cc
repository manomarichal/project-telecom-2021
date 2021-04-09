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



    IPAddress groupaddr = IPAddress(arg_list[0]);
    bool exists = false;
    for(int i = 0; i<= memReportMsg.group_records.size(); i++){
        if (memReportMsg.group_records[i].multicast_adress == groupaddr){
            //check if the client is a part of this group record already
            for (int y =0; y<=memReportMsg.group_records[i].source_adresses.size(); y++){
                if (memReportMsg.group_records[i].source_adresses[y] == clientIP){
                    //case if the client has already joined the group address
                    //the mc address exists and the client is already a part of it
                    return -1;
                }
            }
            //if the client is not a part off the group, add it to the group
            memReportMsg.group_records[i].source_adresses.push_back(clientIP);
            memReportMsg.group_records[i].number_of_sources++;
            exists = true;
        }
    }
    if(!exists){
        //make new group record if it does not yet exist
        igmp_group_record grRecord;
        //the group record needs to be made
        grRecord.record_type = 0;
        grRecord.multicast_adress = groupaddr;
        grRecord.number_of_sources = 1;
        grRecord.source_adresses.push_back(clientIP);
        grRecord.mode = exclude
        memReportMsg.group_records.push_back(grRecord);
    }
    element->make_mem_report_packet();
    element->output(0).push(p);
    click_chatter("completed join");
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
WritablePacket * IGMPClientSide::make_mem_report_packet()
{
    click_chatter("creating membership report");

    uint32_t size = sizeof(click_ip) + sizeof(memReportMsg); // TODO is this the correct way of doing it
    WritablePacket *p = Packet::make(size);
    memset(p->data(), '\0', p->length()); // TODO why, they do it in icmpsendpings

    // IP HEADER, based on elements/icmp/icmpsendpings.cc, line 133
    click_ip *nip = reinterpret_cast<click_ip *>(p->data()); // place ip header at data pointer
    nip->ip_v = 4;
    nip->ip_hl = (sizeof(click_ip)) >> 2; //TODO why shift two bits?
    nip->ip_len = htons(p->length()); // TODO why only hton lenght
    uint16_t ip_id = 1; // TODO does it matter?
    nip->ip_id = htons(ip_id); // converts host byte order to network byte order
    nip->ip_p = IP_PROTO_IGMP; // must be 2, check ipadress/clicknet/ip.h, line 56
    nip->ip_ttl = 1; // specified in RFC3376 page 7
    nip->ip_src = IPAddress(clientIP);
    nip->ip_dst = IPAddress(("224.0.0.22")); // all multicast routers listen to this adress
    nip->ip_sum = click_in_cksum((unsigned char *)nip, sizeof(click_ip)); // TODO no idea

    // IP OPTIONS, every packet needs an IP Router Alert option [RFC-2113], specified in RFC3376 page 7
    // TODO ??? do we make a struct ourselves or is there something for this

    // add IGMP membership reports, based on how they add the icmp strutcs in elements/icmp/icmpsendpings.cc, line 145
    igmp_mem_report_msg *igmp_p = (struct igmp_mem_report_msg *) (nip + 1); // place igmp data after the ip header
    igmp_p->number_of_group_records = 0; //TODO
    igmp_p-> checksum = 0; //TODO
    igmp_p-> group_records = memReportMsg.group_records;

    // finishing up
    p->set_dst_ip_anno(IPAddress(("224.0.0.22")));
    p->set_ip_header(nip, sizeof(click_ip));
    p->timestamp_anno().assign_now();

    return p;
}

void IGMPClientSide::push(int, Packet *p)
{
    // TODO needs to accept and process queries (also something about udp)
    // unpacking data, based on elements/icmp/icmpsendpings.cc, line 194
    const click_ip *iph = p->ip_header();
    // TODO how to unpack
    click_chatter("packet recieved");
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

