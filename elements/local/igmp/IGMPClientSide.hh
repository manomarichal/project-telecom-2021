#ifndef CLICK_IGMPCLIENTSIDE_HH
#define CLICK_IGMPCLIENTSIDE_HH

#include <click/element.hh>
#include "igmp.h"
#include "IGMPV3ReportHelper.hh"
#include "IGMPV3QueryHelper.hh"
#include <click/timer.hh>

CLICK_DECLS

class IGMPClientSide : public Element {
public:
    struct URI_packages{
        IGMPClientSide* client;
        WritablePacket *p;
        int RV;
        uint32_t uri;
    };

    IGMPClientSide();

    ~IGMPClientSide();

    const char *class_name() const { return "IGMPClientSide"; }

    const char *port_count() const { return "2/3"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet *);

    //static handles, important will not work otherwise
    //found in ipsecroutetable.cc/hh
    static int client_join(const String &conf, Element *e, void *thunk, ErrorHandler *errh);

    static int client_leave(const String &conf, Element *e, void *thunk, ErrorHandler *errh);

    void add_handlers();

    void print_group_records();

    void run_timer(Timer *);


private:
    static void URI_timer(Timer *, void *);
    uint32_t robustness; // TODO not hardcoded
    uint32_t unsolicited_report_interval; // TODO not hardcoded
    uint32_t max_size;
    igmp_mem_report mem_report;
    Vector <igmp_group_record> group_records;

    IPAddress clientIP;
    IPAddress MC_ADDRESS;
    IPAddress ALL_SYSTEMS_MC_ADDRESS;
    IGMPV3ReportHelper *helper = new IGMPV3ReportHelper();
    int _local_timer = 0;
    int _query_timer = 0;

    uint32_t _get_size_of_igmp_data();

    WritablePacket *make_mem_report_packet();

    Timer _timer;
    Vector <to_send> queue;
    to_send currently_sending;
    Vector <URI_packages> URI_messages;
    IGMPV3QueryHelper *query_helper = new IGMPV3QueryHelper();

};

CLICK_ENDDECLS
#endif

