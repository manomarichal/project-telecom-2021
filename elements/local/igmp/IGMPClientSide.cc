#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "IGMPClientSide.hh"

CLICK_DECLS
IGMPClientSide::IGMPClientSide()
{}

IGMPClientSide::~IGMPClientSide()
{}

void IGMPClientSide::client_join() {
    click.chatter("hallo")
}

void IGMPClientSide::client_join() {
    click.chatter("hallo")
}

void IGMPClientSide::add_handlers() {
    add_write_handler("client_join", client_join, (void *) 0)
//    add_write_handler("client_leave")
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

