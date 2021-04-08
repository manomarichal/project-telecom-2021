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

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide)

