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

/**
 * enum containing the different filter modes
 */
enum filter_mode {
    include = 1, exclude = 2, change_to_include = 3, change_to_exclude = 4
};

#define IGMP_V3_INCLUDE 1
#define IGMP_V3_EXCLUDE 2
#define IGMP_V3_CHANGE_TO_INCLUDE 3
#define IGMP_V3_CHANGE_TO_EXCLUDE 4

/**
 * ipaddress struct, typo is on purpose
 */
struct ipadress {
    uint32_t adress;
};

/**
 * router alert struct based on RFC-2113
 */
struct router_alert {
    uint8_t field_1;
    uint8_t field_2;
    uint16_t octet;
};

/**
 * struct used to represent a single group record, see RFC3367 page 14
 */
struct igmp_group_record_message {
    uint8_t record_type; // see RFC3376 page 16
    const uint8_t aux_data_len = 0; // IGMPv3 does not define any auxiliary data thus length must be zero
    uint16_t number_of_sources;
    uint32_t multicast_adress;
};

/**
 * struct to represent the group record, sources is not really used in this project :(
 */
struct igmp_group_record {
    IPAddress multicast_adress;
    uint16_t number_of_sources;
    uint8_t record_type; // see RFC3376 page 16
    const uint8_t aux_data_len = 0; // IGMPv3 does not define any auxiliary data thus length must be zero
    Vector <IPAddress> sources;
};

#define IGMP_V3_MEM_RECORD 0x22
#define IGMP_MEM_QUERY 0x11

/**
 * struct used to represent a membership report, see RFC3367 page 13
 */
struct igmp_mem_report {
    uint8_t type;
    const uint8_t reserved_1 = 0;
    uint16_t checksum;
    const uint16_t reserved2 = 0;
    uint16_t number_of_group_records;
};

/**
 * struct used to represent a membership query, see RFC3367 page 9
 */
struct igmp_mem_query_msg {
    uint8_t type = IGMP_MEM_QUERY; // indicate its a membership query
    uint8_t max_resp_code; // max amount of time allowed to respond
    uint16_t checksum;
    uint32_t group_adress;
    //RV in router
    unsigned qrv:3;
    unsigned s:1;
    unsigned resv:4;
    //QI in router
    uint8_t qqic; // queries query interval
    uint16_t number_of_sources;
};

/**
 * struct for source record, contains adress and timer used later in the project, RFC page 26
 */
struct igmp_source_record {
    IPAddress source_address;
    uint32_t source_timer;
};

/**
 * struct for group state used in router, RFC page 26
 */
struct igmp_group_state {
    Vector <igmp_source_record> source_records;
    uint8_t mode;
    IPAddress multicast_adress;
};


#endif //PROJECT_TELECOM_IGMP_H
