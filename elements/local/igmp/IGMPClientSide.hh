#ifndef CLICK_IGMPCLIENTSIDE_HH
#define CLICK_IGMPCLIENTSIDE_HH
#include <click/element.hh>
#include "igmp.h"
CLICK_DECLS

class IGMPClientSide : public Element {
public:
    IGMPClientSide();
    ~IGMPClientSide();

    const char *class_name() const	{ return "IGMPClientSide"; }
    const char *port_count() const	{ return "1/1"; }
    const char *processing() const	{ return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);
//not implemented, causes linking error
//    void push(int, Packet *);

    //static handles, important will not work otherwise
    //found in ipsecroutetable.cc/hh
    static int client_join(const String &conf, Element *e, void *thunk, ErrorHandler *errh);
    static int client_leave(const String &conf, Element *e, void *thunk, ErrorHandler *errh);
    void add_handlers();

private:
    uint32_t maxSize;
    igmp_mem_report_msg memReportMsg;
    Packet* make_membership_packet();
    IPAddress clientIP;
};

CLICK_ENDDECLS
#endif

