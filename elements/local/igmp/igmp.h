// =====================================================================
// @name: igmp.h
// @project: project-telecom
// @author: Mano Marichal
// @date: 08.04.21
// @copyright: BA2 Informatica - Mano Marichal - University of Antwerp
// @description: some structs
// =====================================================================

#ifndef PROJECT_TELECOM_IGMP_H
#define PROJECT_TELECOM_IGMP_H

#include <click/ipaddress.hh>
#include <map>

enum filter_mode{include, exclude};
// struct used to represent a single group record, see RFC3367 page 14

struct ipadress
{
    uint32_t adress;
};

struct igmp_group_record_message
{
    uint8_t record_type; // see RFC3376 page 16
    const uint8_t aux_data_len = 0; // IGMPv3 does not define any auxiliary data thus length must be zero
    uint16_t number_of_sources;
    filter_mode mode;
    uint32_t multicast_adress;
};

struct igmp_group_record
{
    IPAddress multicast_adress;
    uint16_t number_of_sources;
    uint8_t record_type; // see RFC3376 page 16
    const uint8_t aux_data_len = 0; // IGMPv3 does not define any auxiliary data thus length must be zero
    filter_mode mode;
    Vector<IPAddress> sources;
};

#define IGMP_V3_MEM_RECORD 0x22
#define IGMP_MEM_QUERY 0x11

// struct used to represent a membership report, see RFC3367 page 13
struct igmp_mem_report
{
    const uint8_t type = IGMP_V3_MEM_RECORD;
    const uint8_t reserved_1 = 0;
    const uint16_t reserved2 = 0;
    uint16_t number_of_group_records;
    uint16_t checksum;
};

// struct used to represent a membership query, see RFC3367 page 9
struct igmp_mem_query_msg
{
    const uint8_t type = IGMP_MEM_QUERY; // indicate its a membership query
    uint8_t max_resp_code; // max amount of time allowed to respond
    uint16_t checksum;
    uint32_t group_adress;
    // TODO resv & S & QRV
    uint8_t qqic = 0; // queries query interval
    uint16_t number_of_sources;
};
#endif //PROJECT_TELECOM_IGMP_H
