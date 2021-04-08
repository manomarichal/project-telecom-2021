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
struct igmp_group_record_message
{
    uint8_t record_type; // see RFC3376 page 16
    const uint8_t aux_data_len = 0; // IGMPv3 does not define any auxiliary data thus length must be zero
    uint16_t number_of_sources;
    uint32_t multicast_adress;
    Vector<uint32_t> source_adresses; // maybe use arrays here to avoid sending extra ptrs
};

// struct used to represent a membership report, see RFC3367 page 13
WriteablePacket *
struct igmp_membership_report
{
    const uint8_t type = 0x22; // indicate its a membership record
    const uint8_t reserved_1 = 0;
    uint16_t number_of_group_records;
    uint16_t checksum;
    const uint16_t reserved2 = 0;
};

#endif //PROJECT_TELECOM_IGMP_H
