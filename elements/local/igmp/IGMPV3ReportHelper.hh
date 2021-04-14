//
// Created by student on 4/13/21.
//

#ifndef CLICK_IGMPV3REPORTHELPER_HH
#define CLICK_IGMPV3REPORTHELPER_HH

#include <click/element.hh>
#include"./igmp.h"

CLICK_DECLS

// using namespace makes click mad, so class with static functions it is
class IGMPV3ReportHelper : public Element {
public:
    IGMPV3ReportHelper();
    ~IGMPV3ReportHelper();

    const char *class_name() const	{ return "IGMPV3ReportHelper"; }
    const char *port_count() const	{ return "0/0"; }
    const char *processing() const	{ return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);
    void push(int, Packet *);

    uint32_t get_size_of_data(const Vector<igmp_group_record> group_records);
    click_ip* add_ip_header(WritablePacket* p, IPAddress client_ip, IPAddress multicast_address, bool verbose);
    router_alert * add_router_alert(void *start, uint8_t octet_1=0, uint8_t octet_2=0);
    igmp_mem_report* add_igmp_data(void *start, const Vector<igmp_group_record> group_records);
    igmp_mem_report igmp_unpack_info(const void *start);
    Vector<igmp_group_record> igmp_unpack_group_records(const void *start, uint16_t number_of_group_records);
};

CLICK_ENDDECLS
#endif