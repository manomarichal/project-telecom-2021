//
// Created by student on 4/13/21.
//

#ifndef CLICK_IGMPV3QUERYHELPER_HH
#define CLICK_IGMPV3QUERYHELPER_HH

#include <click/element.hh>
#include"./igmp.h"

CLICK_DECLS

/**
 * helper class which does anything v3 packet related like adding data and unpacking data into the igmp structs
 */
class IGMPV3QueryHelper : public Element {
public:
    IGMPV3QueryHelper();

    ~IGMPV3QueryHelper();

    const char *class_name() const { return "IGMPV3QueryHelper"; }

    const char *port_count() const { return "0/0"; }

    const char *processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler *);

    void push(int, Packet *);

    uint32_t get_size_of_data(uint16_t number_of_sources);

    click_ip *add_ip_header(WritablePacket *p, IPAddress source, IPAddress destination, bool verbose = true);

    router_alert *add_router_alert(void *start, uint8_t octet_1 = 0, uint8_t octet_2 = 0);

    igmp_mem_query_msg *add_igmp_data(void *start, const Vector <IPAddress> source_addresses, IPAddress group_address,
                                      bool general, unsigned qrv, unsigned qqic, uint8_t max_resp_code);

    igmp_mem_query_msg unpack_query_data(void *start);
//    igmp_mem_report igmp_unpack_info(const void *start);

//    Vector <igmp_group_record> igmp_unpack_group_records(const void *start, uint16_t number_of_group_records);
};

CLICK_ENDDECLS
#endif