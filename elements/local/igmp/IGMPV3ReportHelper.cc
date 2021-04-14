//
// Created
// by student on 4/14/21.
//
#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include <clicknet/ip.h>
#include "IGMPV3ReportHelper.hh"

CLICK_DECLS
IGMPV3ReportHelper::IGMPV3ReportHelper()
{}

IGMPV3ReportHelper::~IGMPV3ReportHelper()
{}

int IGMPV3ReportHelper::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return 0;
}

void IGMPV3ReportHelper::push(int port, Packet *p)
{

}

uint32_t IGMPV3ReportHelper::get_size_of_data(const Vector<igmp_group_record> group_records)
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

click_ip* IGMPV3ReportHelper::add_ip_header(WritablePacket* p, IPAddress client_ip, IPAddress multicast_address, bool verbose)
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

igmp_mem_report * IGMPV3ReportHelper::add_igmp_data(void *start, const Vector<igmp_group_record> group_records)
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
                igmp_adr = (struct ipadress *) (igmp_adr + 1);
            }
        }

        // move pointer to add a new info
        if (i < group_records.size()-1)
        {
            igmp_grp = (struct igmp_group_record_message*)(igmp_adr + 1);
        }

        // igmp_mr->checksum = click_in_cksum((unsigned char *)igmp_mr, _get_size_of_igmp_data());
    }
    return igmp_mr;
}

router_alert * IGMPV3ReportHelper::add_router_alert(void *start, uint8_t octet_1, uint8_t octet_2)
{
    {
        // add ip option
        router_alert* r_alert = reinterpret_cast<router_alert*>(start);
        r_alert->field_1 = 0b10010100; // see RFC-2113
        r_alert->field_2 = 0b00000100; // see RFC-2113
        r_alert->octet = ((uint16_t)octet_2<< 8) | octet_1;

        return r_alert;
    }
}

Vector<igmp_group_record>  IGMPV3ReportHelper::igmp_unpack_group_records(const void *start, uint16_t number_of_group_records)
{
    Vector<igmp_group_record> records;
    const igmp_group_record_message *group_record = reinterpret_cast<const igmp_group_record_message*>(start);
    for (int i=0;i<number_of_group_records;i++)
    {
        igmp_group_record new_group_record;
        new_group_record.multicast_adress = group_record->multicast_adress;
        // record.mode = group->mode; what
        new_group_record.record_type = group_record->record_type;
        new_group_record.number_of_sources = group_record->number_of_sources;

        const ipadress *source = (struct ipadress*)(group_record+1);
        for (int j=0;j<new_group_record.number_of_sources;j++)
        {
            new_group_record.sources.push_back(IPAddress(source->adress));
            if (i < new_group_record.number_of_sources-1)
            {
                source = (struct ipadress *) (source + 1);
            }
        }

        if (i < number_of_group_records-1)
        {
            group_record = (struct igmp_group_record_message*)(source + 1);
        }
        records.push_back(new_group_record);
    }
    return records;
}

igmp_mem_report IGMPV3ReportHelper::igmp_unpack_info(const void *start)
{
    igmp_mem_report report;
    const igmp_mem_report *igmp_mr = reinterpret_cast<const igmp_mem_report*>(start);
    report.type = igmp_mr->type;
    report.checksum = igmp_mr->checksum;
    report.number_of_group_records = igmp_mr->number_of_group_records;
    return report;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPV3ReportHelper) // forces to create element within click