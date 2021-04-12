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
    void push(int, Packet *);

    //static handles, important will not work otherwise
    //found in ipsecroutetable.cc/hh
    static int client_join(const String &conf, Element *e, void *thunk, ErrorHandler *errh);
    static int client_leave(const String &conf, Element *e, void *thunk, ErrorHandler *errh);
    void add_handlers();

private:
    uint32_t robustness = 1; // TODO not hardcoded
    uint32_t report_interval = 10; // TODO not hardcoded
    uint32_t max_size;
    igmp_mem_report mem_report;
    Vector<igmp_group_record> group_records;
    void* _add_igmp_data(void *start);
    click_ip* _add_ip_header(void* start);
    uint32_t _get_size_of_igmp_data();
    // packet functions
    WritablePacket* make_mem_report_packet();
    /*
    click_ip * ip_encap(WritablePacket *p);
    void * igmp_encap(click_ip *p)
    */
    IPAddress clientIP;
    IPAddress MC_ADDRESS;
    IPAddress ALL_SYSTEMS_MC_ADDRESS;
};

CLICK_ENDDECLS
#endif

