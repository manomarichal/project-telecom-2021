#ifndef CLICK_IGMPCLIENTSIDE_CC
#define CLICK_IGMPCLIENTSIDE_CC
#include "IGMPClientSide.hh"

CLICK_DECLS


void IGMPClientSide::client_join() {
    click.chatter("hallo")
}

void IGMPClientSide::add_handlers() {
    add_write_handler("client_join", client_join, (void*) 0)
//    add_write_handler("client_leave")
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

































CLICK_ENDDECLS
#endif

