#ifndef CLICK_IGMPROUTERSIDE_HH
#define CLICK_IGMPROUTERSIDE_HH
#include <click/element.hh>
#include "igmp.h"
CLICK_DECLS

class IGMPRouterSide : public Element {
public:
    IGMPRouterSide();
    ~IGMPRouterSide();

    const char *class_name() const	{ return "IGMPRouterSide"; }
    const char *port_count() const	{ return "3/3"; }
    const char *processing() const	{ return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet *);

private:
    IPAddress routerIP;
    Vector<igmp_group_state> group_states;
};

CLICK_ENDDECLS
#endif

