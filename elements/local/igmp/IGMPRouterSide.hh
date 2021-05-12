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
    IGMPRouterSide();

    ~IGMPRouterSide();

    const char *class_name() const { return "IGMPRouterSide"; }

    const char *port_count() const { return "3/9"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void run_timer(Timer *);

    void push(int, Packet *);

private:
    void update_group_states(const click_ip *ip_header, Vector <igmp_group_record> group_records,  int port);

    //void update_group_state(const click_ip *ip_header, igmp_group_state state, igmp_group_record record);

    void multicast_udp_packet(Packet *p, int port);
    WritablePacket *make_general_query_packet();
    WritablePacket *make_group_specific_query_packet();
    IGMPV3QueryHelper *query_helper = new IGMPV3QueryHelper();
    IGMPV3ReportHelper *report_helper = new IGMPV3ReportHelper();

    IPAddress routerIP;
    Vector <Vector<igmp_group_state>> interface_states;
    Vector <IPAddress> receivers;
    unsigned robustness_variable;
    unsigned query_interval;
    unsigned max_response_time;
    int _count;
    Timer _timer;
    IPAddress ASMC_ADDRESS= IPAddress("224.0.0.1");
    IPAddress MC_ADDRESS= IPAddress("224.0.0.22");


};

CLICK_ENDDECLS
#endif

