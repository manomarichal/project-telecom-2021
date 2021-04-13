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
    click_chatter("router recieved packet type %i in port %i ", iph->ip_p, port);

    if(port==0){
        click_chatter("regular packet");

    }
    if(port==1){
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
        click_chatter("igmp packet");
        //input port of the udp messages
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterSide) // forces to create element within click
