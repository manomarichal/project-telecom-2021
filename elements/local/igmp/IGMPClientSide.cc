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

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

