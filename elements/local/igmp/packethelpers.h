//
// Created by student on 4/13/21.
//

#ifndef PROJECT_TELECOM_2021_PACKETHELPERS_H
#define PROJECT_TELECOM_2021_PACKETHELPERS_H

#include"./igmp.h"

namespace igmp_v3_reports{
    uint32_t get_size_of_data(const Vector<igmp_group_record> group_records)
    {
        int32_t size = 0;
        size += sizeof(igmp_mem_report);
        for(int i=0;i<group_records.size(); i++)
        {
            size += sizeof(igmp_group_record_message);
            for (int j=0;j<group_records[i].sources.size(); j++)
            {
                size += sizeof(ipadress);
            }
        }
        return size;
    }

    click_ip* add_ip_header(WritablePacket* p, IPAddress client_ip, IPAddress multicast_address, bool verbose)
    {
        // based on elements/icmp/icmpsendpings.cc, line 133
        click_ip *nip = reinterpret_cast<click_ip*>(p->data()); // place ip header at data pointer
        nip->ip_v = 4;
        nip->ip_hl = (sizeof(click_ip) + sizeof(router_alert)) >> 2; // 4 because of router alert, idk why right shift by 2
        nip->ip_len = htons(p->length());
        uint16_t ip_id = 1; // does not matter
        nip->ip_id = htons(ip_id); // converts host byte order to network byte order
        nip->ip_p = IP_PROTO_IGMP; // must be 2, check ipadress/clicknet/ip.h, line 56
        nip->ip_ttl = 1; // specified in RFC3376 page 7
        nip->ip_src = client_ip.in_addr();
        nip->ip_dst = multicast_address.in_addr(); // all multicast routers listen to this adress
        nip->ip_sum = click_in_cksum((unsigned char *) nip, sizeof(click_ip) + sizeof(router_alert)); // copy paste from icmpsendpings.cc

        if (!verbose)
        {
            click_chatter("printing click ip while making report message: ");
            click_chatter("\t ip_src %s",client_ip.unparse().c_str());
            click_chatter("\t ip_dst %s",multicast_address.unparse().c_str());
            click_chatter("\t ip_v %d",nip->ip_v);
            click_chatter("\t ip_hl %d",nip->ip_hl);
            click_chatter("\t ip_len %d",nip->ip_len);
            click_chatter("\t ip_id %d",nip->ip_id);
            click_chatter("\t ip_p %d",nip->ip_p);
            click_chatter("\t ip_ttl %d",nip->ip_ttl);
            click_chatter("\t ip_sum %d",nip->ip_sum);
        }
        return nip;
    }

    router_alert * add_router_alert(void *start, uint8_t octet_1=0, uint8_t octet_2=0)
    {
        // add ip option
        router_alert* r_alert = reinterpret_cast<router_alert*>(start);
        r_alert->field_1 = 0b10010100; // see RFC-2113
        r_alert->field_2 = 0b00000100; // see RFC-2113
        r_alert->octet = ((uint16_t)octet_2<< 8) | octet_1;

        return r_alert;
    }

    igmp_mem_report* add_igmp_data(void *start, const Vector<igmp_group_record> group_records)
    {
        // add the membership report info
        igmp_mem_report *igmp_mr = reinterpret_cast<igmp_mem_report*>(start);
        igmp_mr->number_of_group_records = htons(group_records.size());
        igmp_mr->type = IGMP_V3_MEM_RECORD;
        if (group_records.size() == 0) {return igmp_mr;};
        igmp_group_record_message *igmp_grp = (struct igmp_group_record_message*)(igmp_mr + 1);
        for(int i=0;i<group_records.size(); i++)
        {
            click_chatter("adding igmp data for group %d", i);
            // set the fields in the reserved space to the correct thing
            igmp_grp->multicast_adress = group_records[i].multicast_adress.addr();
            igmp_grp->number_of_sources = group_records[i].number_of_sources;
            igmp_grp->record_type = group_records[i].record_type;

            // add source adresses on top

            ipadress *igmp_adr = (struct ipadress *) (igmp_grp + 1);
            for (int j=0;j<group_records[i].sources.size(); j++)
            {
                igmp_adr->adress = group_records[i].sources[j].addr();
                // move pointer
                if (i < group_records.size()-1)
                {
                    igmp_adr = (struct ipadress *) (igmp_grp + 1);
                }
            }

            // move pointer to add a new info
            if (i < group_records.size()-1)
            {
                igmp_grp = (struct igmp_group_record_message*)(igmp_grp + 1);
            }

            // igmp_mr->checksum = click_in_cksum((unsigned char *)igmp_mr, _get_size_of_igmp_data());
        }
        return igmp_mr;
    }
    igmp_mem_report igmp_unpack_info(void *start)
    {
        igmp_mem_report report;
        igmp_mem_report *igmp_mr = reinterpret_cast<igmp_mem_report*>(start);
        report.type = igmp_mr->type;
        report.checksum = igmp_mr->checksum;
        report.number_of_group_records = igmp_mr->number_of_group_records;
        return report;
    }

    Vector<igmp_group_record> igmp_unpack_group_records(void *start)
    {
        Vector<igmp_group_record> records;
        igmp_mem_report *igmp_mr = reinterpret_cast<igmp_mem_report*>(start);
    }
}

#endif //PROJECT_TELECOM_2021_PACKETHELPERS_H
