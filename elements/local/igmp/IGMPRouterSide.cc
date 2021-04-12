//
// Created by Gebruiker on 7/04/2021.
//
#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
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

void IGMPRouterSide::push(int port, Packet *p){
    const click_ip *iph = p->ip_header();
    if(port==0){
        //input port of the igmp messages
    }
    if(port==1){
        //input port of the udp messages
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterSide) // forces to create element within click
