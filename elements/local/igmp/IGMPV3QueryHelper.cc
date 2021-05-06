//
// Created by student on 5/6/21.
//

#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include <clicknet/ip.h>
#include "./IGMPV3QueryHelper.hh"

/**
 * adds ip header, thus kind of initialisation
 * @param p the packet that needs ipheader
 * @param client_ip clients ip address
 * @param multicast_address the multicast address
 * @param verbose if you want cmd outputs or not
 * @return a click_ip object
 */
CLICK_DECLS
IGMPV3QueryHelper::IGMPV3QueryHelper() {}

IGMPV3QueryHelper::~IGMPV3QueryHelper() {}

int IGMPV3QueryHelper::configure(Vector <String> &conf, ErrorHandler *errh) {
return 0;
}

/**
 * needs to be here to be considered an element
 * @param port
 * @param p
 */
void IGMPV3QueryHelper::push(int port, Packet *p) {

}

click_ip *IGMPV3QueryHelper::add_ip_header(WritablePacket *p, IPAddress source, IPAddress destination, bool verbose) {
    // based on elements/icmp/icmpsendpings.cc, line 133
    click_ip *nip = reinterpret_cast<click_ip *>(p->data()); // place ip header at data pointer
    nip->ip_v = 4;
    nip->ip_hl = (sizeof(click_ip) + sizeof(router_alert)) >> 2; // 4 because of router alert, idk why right shift by 2
    nip->ip_len = htons(p->length());
    uint16_t ip_id = 1; // does not matter
    nip->ip_id = htons(ip_id); // converts host byte order to network byte order
    nip->ip_p = IP_PROTO_IGMP; // must be 2, check ipadress/clicknet/ip.h, line 56
    nip->ip_ttl = 1; // specified in RFC3376 page 7
    nip->ip_src = source.in_addr();
    nip->ip_dst = destination.in_addr(); // all multicast routers listen to this adress
//    nip->ip_sum = click_in_cksum((unsigned char *) nip, sizeof(click_ip) + sizeof(router_alert)); // copy paste from icmpsendpings.cc

    if (verbose) {
        click_chatter("printing click ip while making report message: ");
        click_chatter("\t ip_src %s", source.unparse().c_str());
        click_chatter("\t ip_dst %s", destination.unparse().c_str());
        click_chatter("\t ip_v %d", nip->ip_v);
        click_chatter("\t ip_hl %d", nip->ip_hl);
        click_chatter("\t ip_len %d", nip->ip_len);
        click_chatter("\t ip_id %d", nip->ip_id);
        click_chatter("\t ip_p %d", nip->ip_p);
        click_chatter("\t ip_ttl %d", nip->ip_ttl);
        click_chatter("\t ip_sum %d", nip->ip_sum);
    }
    return nip;
}

uint32_t IGMPV3QueryHelper::get_size_of_data(uint16_t number_of_sources) {
    return sizeof(igmp_mem_query_msg) + number_of_sources * sizeof(ipadress);
}

router_alert *IGMPV3QueryHelper::add_router_alert(void *start, uint8_t octet_1, uint8_t octet_2) {
    {
        // add ip option
        router_alert *r_alert = reinterpret_cast<router_alert *>(start);
        r_alert->field_1 = 0b10010100; // see RFC-2113
        r_alert->field_2 = 0b00000100; // see RFC-2113
        r_alert->octet = ((uint16_t) octet_2 << 8) | octet_1;

        return r_alert;
    }
}

igmp_mem_query_msg *IGMPV3QueryHelper::add_igmp_data(void *start, const Vector <IPAddress> source_addresses, IPAddress group_address) {
    // add the membership report info
    igmp_mem_query_msg *query = reinterpret_cast<igmp_mem_query_msg *>(start);
    query->max_resp_code = 0;
    query->type = IGMP_MEM_QUERY;
    query->group_adress = group_address.addr();
    query->resv_s_qrv = 0;
    query->qqic = 0; // queries query interval
    query->number_of_sources = source_addresses.size();

    // add source adresses on top
    ipadress *igmp_adr = (struct ipadress *) (query + 1);
    for (int i = 0; i < source_addresses.size(); i++) {
        igmp_adr->adress = source_addresses[i].addr();
        // move pointer
        if (i < source_addresses.size() - 1) {
            igmp_adr = (struct ipadress *) (igmp_adr + 1);
        }
    }
    query->checksum = click_in_cksum((unsigned char *) query, get_size_of_data(source_addresses.size()));
    return query;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPV3ReportHelper) // forces to create element within click