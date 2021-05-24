#ifndef CLICK_IGMPROUTERSIDE_HH
#define CLICK_IGMPROUTERSIDE_HH

#include <click/element.hh>
#include <click/timer.hh>
#include "./IGMPV3QueryHelper.hh"
#include "./IGMPV3ReportHelper.hh"
#include "igmp.h"

CLICK_DECLS

class IGMPRouterSide : public Element {
public:
    struct query_timer{
        IGMPRouterSide* router;
    };

    struct specific_query_timer{
        IGMPRouterSide* router;
    };

    IGMPRouterSide();

    ~IGMPRouterSide();

    const char *class_name() const { return "IGMPRouterSide"; }

    const char *port_count() const { return "3/9"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void run_timer(Timer *);

    void push(int, Packet *);

private:


    static void general_query_timer(Timer *, void *);
    static void group_specific_query_timer(Timer *, void *);
    static void second_group_specific_query_timer(Timer *, void *);

    unsigned robustness_variable;
    unsigned query_interval;
    unsigned GMI;
    unsigned startup_interval;
    unsigned startup_count;
    unsigned LMQI;
    unsigned LMQC;
    unsigned LMQT;
    unsigned QRI;

    IPAddress routerIP;
    IPAddress ASMC_ADDRESS= IPAddress("224.0.0.1");
    IPAddress MC_ADDRESS= IPAddress("224.0.0.22");
    Vector <Vector<igmp_group_state>> interface_states;
    Vector <IPAddress> receivers;

    Timer* specific_timer;
    Timer* second_specific_timer;
    Timer _timer;

    void process_report(igmp_group_record *record, int port);
    void multicast_udp_packet(Packet *p);
    WritablePacket *make_general_query_packet();
    WritablePacket *make_group_specific_query_packet(IPAddress group_address, int interface);
    IGMPV3QueryHelper *query_helper = new IGMPV3QueryHelper();
    IGMPV3ReportHelper *report_helper = new IGMPV3ReportHelper();
};

CLICK_ENDDECLS
#endif

