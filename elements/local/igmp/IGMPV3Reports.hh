//
// Created by student on 4/13/21.
//

#ifndef CLICK_IGMPV3REPORTHELPER_HH
#define CLICK_IGMPV3REPORTHELPER_HH

#include"./igmp.h"
#include <click/element.hh>
CLICK_DECLS

// using namespace makes click mad, so class with static functions it is
class IGMPV3ReportHelper : public Element {
    IGMPV3ReportHelper();
    ~IGMPV3ReportHelper();
public:
    static uint32_t get_size_of_data(const Vector<igmp_group_record> group_records);
    static click_ip* add_ip_header(WritablePacket* p, IPAddress client_ip, IPAddress multicast_address, bool verbose);
    static router_alert * add_router_alert(void *start, uint8_t octet_1=0, uint8_t octet_2=0);
    static igmp_mem_report* add_igmp_data(void *start, const Vector<igmp_group_record> group_records);
    static igmp_mem_report igmp_unpack_info(const void *start);
    static Vector<igmp_group_record> igmp_unpack_group_records(const void *start, uint16_t number_of_group_records);
};

CLICK_ENDDECLS
#endif