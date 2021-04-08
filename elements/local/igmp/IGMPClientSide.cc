#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
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
    click_chatter("hallo, join");
    return 0;
}

int IGMPClientSide::client_leave(const String &conf, Element *e, void *thunk, ErrorHandler *errh) {
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

    // IP HEADER (copy pasted/edited slightly from ements/icmp/icmpsendpings.cc)
    click_ip *ip_header = reinterpret_cast<click_ip *>(q->data());
    ip_header->ip_v = 4;
    ip_header->ip_hl = sizeof(click_ip) >> 2;
    ip_header->ip_len = htons(p->length());
    uint16_t ip_id = (_count % 0xFFFF) + 1; // ensure ip_id != 0
    ip_header->ip_id = htons(ip_id);
    ip_header->ip_p = 2 // specified in RFC3376 page 7
    ip_header->ip_ttl = 1; // specified in RFC3376 page 7
    ip_header->ip_src = _src; // source adress of this element
    ip_header->ip_dst = IPAdress("224.0.0.22"); // all multicast routers listen to this adress
    ip_header->ip_sum = click_in_cksum((unsigned char *)nip, sizeof(click_ip));

}
CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

