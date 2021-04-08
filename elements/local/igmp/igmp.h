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

// struct used to represent a single group record for a client, see RFC3367 page 14 for more info
struct igmp_group_record
{
    uint32_t aux_data_len;
    uint32_t number_of_sources;
    IPAdress source_adress;

};

#endif //PROJECT_TELECOM_IGMP_H
