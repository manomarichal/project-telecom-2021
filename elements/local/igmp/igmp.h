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

// struct used to represent a single group record for a client, see RFC3367 page 14

struct igmp_group_record
{
    uint32_t record_type; // see RFC3376 page 16
    const uint32_t aux_data_len = 0; //IGMPv3 does not define any auxiliary data thus lenght must be zero
    uint32_t number_of_sources;
    IPAddress multicast_adress;
    Vector<IPAddress> source_adresses;
};

#endif //PROJECT_TELECOM_IGMP_H
