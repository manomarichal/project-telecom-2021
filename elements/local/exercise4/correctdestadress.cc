#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "correctdestadress.hh"
#include "../../ip/fixipsrc.hh"
CLICK_DECLS
CorrectDestAdress::CorrectDestAdress()
{}

CorrectDestAdress::~ CorrectDestAdress()
{}

int CorrectDestAdress::configure(Vector<String> &conf, ErrorHandler *errh) {
    IPAddress a;
    if (Args(conf, this, errh).read_mp("IPADDR", a).complete() < 0)
        return -1;
    _my_ip = a.in_addr();
    return 0;
}

void CorrectDestAdress::push(int, Packet *p_in){
    WritablePacket *p = p_in->uniqueify();
    click_ip *ip = p->ip_header();

    ip->ip_dst = _my_ip;
    int hlen = ip->ip_hl << 2;
    ip->ip_sum = 0;
    ip->ip_sum = click_in_cksum((unsigned char *)ip, hlen);

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CorrectDestAdress)

