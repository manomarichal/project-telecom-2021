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

// struct used to represent a single group record, see RFC3367 page 14
struct igmp_group_record
{
    uint8_t record_type; // see RFC3376 page 16
    const uint8_t aux_data_len = 0; //IGMPv3 does not define any auxiliary data thus length must be zero
    uint16_t number_of_sources;
    uint32_t multicast_adress;
    Vector<IPAddress> source_adresses;
};

// struct used to represent a membership report, see RFC3367 page 13
struct igmp_membership_report
{
    uint8_t type;
    const uint8_t reserved_1 = 0;
    uint16_t number_of_group_records;
    uint16_t checksum;
    const uint16_t reserved1 = 0;
};

#endif //PROJECT_TELECOM_IGMP_H
