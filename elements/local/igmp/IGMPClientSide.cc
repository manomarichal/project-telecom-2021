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
    return 0;
}

int IGMPClientSide::client_join(const String &conf, Element *e, void *thunk, ErrorHandler *errh) {
    click_chatter("hallo");
    return 0;
}

void IGMPClientSide::add_handlers() {
    add_write_handler("client_join", &client_join, (void *)0);
//    add_write_handler("client_leave")
}

// makes membership v3 packets, based on RFC3376 page 13-14
Packet * IGMPClientSide::make_membership_packet()
{
    uint32_t size = 1; // idk yet how to calculate this
    WritablePacket *p = Packet::make(size);
    memset(p->data(), '\0', p->length()); // sets all the data to \0's, ¯\_(ツ)_/¯ but examples do it

    // IP HEADER, based on elements/icmp/icmpsendpings.cc
    click_ip *nip = reinterpret_cast<click_ip *>(p->data());
    nip->ip_v = 4;
    nip->ip_hl = (sizeof(click_ip) + sizeof(IP_options)) >> 2; // IHL field, right shift by 2, ¯\_(ツ)_/¯
    nip->ip_len = htons(p->length()); // converts host byte order to network byte order
    uint16_t ip_id = (_count % 0xFFFF) + 1; // TODO no idea
    nip->ip_id = htons(ip_id); // converts host byte order to network byte order
    nip->ip_p = IP_PROTO_IGMP //TODO found in elements/ip/ipnameinfo.cc, either this or 2 (according to RFC3367 page 7)
    nip->ip_ttl = 1; // specified in RFC3376 page 7
    nip->ip_src = _src; // source adress of this element
    nip->ip_dst = IPAdress(); // all multicast routers listen to this adress
    nip->ip_sum = click_in_cksum((unsigned char *)nip, sizeof(click_ip)); // TODO no idea

    // IP OPTIONS, every packet needs an IP Router Alert option [RFC-2113], specified in RFC3376 page 7
    // geen ide hoe



}
CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

