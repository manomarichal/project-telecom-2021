#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include "IGMPClientSide.hh"

CLICK_DECLS
IGMPClientSide::IGMPClientSide()
{}

IGMPClientSide::~IGMPClientSide()
{}

/*
 * configure for clientside object, should set the clientIP to the given clientaddress
 * might need to add some things later like timer variables
 * CLIENTADDRESS
 * MULTICASTADDRESS
 * ALLSYSTEMSMULTICASTADDRESS
 *    .read_mp("MADDR", MC_ADDRESS)
    .read_mp("ASMADDR",ALL_SYSTEMS_MC_ADDRESS)
 */
int IGMPClientSide::configure(Vector<String> &conf, ErrorHandler *errh)
{
    String ipadresstest;
    String madr;
    String asmadr;
    if (Args(conf, this, errh).read_mp("CADDR", clientIP).read_mp("MADDR", MC_ADDRESS).read_mp("ASMADDR",ALL_SYSTEMS_MC_ADDRESS).complete()<0){click_chatter("failed read, returning 0", ipadresstest); return -1;}
    click_chatter("initialising ipaddress %s", clientIP.unparse().c_str());
    click_chatter("initialising mc address %s", MC_ADDRESS.unparse().c_str());
    click_chatter("initialising asmc address %s", ALL_SYSTEMS_MC_ADDRESS.unparse().c_str());
//    clientIP = IPAddress(ipadresstest);
//    MC_ADDRESS = IPAddress(madr);
//    ALL_SYSTEMS_MC_ADDRESS = IPAddress(asmadr);
    return 0;
}

/*
 * handles the client join for the system.
 * return integer, done throughout other click elements
 *
 */
int IGMPClientSide::client_join(const String &conf, Element *e, __attribute__((unused)) void *thunk, __attribute__((unused)) ErrorHandler *errh) {
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    click_chatter(element->clientIP.unparse().c_str());
    Vector<String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector

    uint32_t groupaddr = IPAddress(arg_list[0]);

    bool exists = false;
    for(int i = 0; i< element->group_records.size(); i++){
        if (element->group_records[i].multicast_adress == groupaddr){
            //check if the client is a part of this group record already
            for (int y =0; y<element->group_records[i].sources.size(); y++){
                if (element->group_records[i].sources[y] == element->clientIP){
                    //case if the client has already joined the group address
                    //the mc address exists and the client is already a part of it
                    click_chatter("The group you are trying to join already has this source address");
                    return -1;
                }
            }
            //if the client is not a part off the group, add it to the group
            element->group_records[i].sources.push_back(element->clientIP);
            element->group_records[i].number_of_sources++;
            exists = true;
        }
    }
    if(!exists){
        //make new group record if it does not yet exist
        igmp_group_record grRecord;
        //the group record needs to be made
        grRecord.record_type = 0;
        grRecord.multicast_adress = groupaddr;
        grRecord.number_of_sources = 1;
        grRecord.sources.push_back(element->clientIP);
        grRecord.mode = exclude;
        element->group_records.push_back(grRecord);
        click_chatter("making a new group record");


//        grRecord.print_record();
    }
    WritablePacket * p =element->make_mem_report_packet();
    element->output(0).push(p);
    click_chatter("completed join");
    element->print_group_records();
    return 0;
}
/*
 * handles the client leave, changes the filter mode fo that group record from exclude to include as defined in RFC
 *
 */
int IGMPClientSide::client_leave(const String &conf, Element *e, __attribute__((unused)) void *thunk, __attribute__((unused)) ErrorHandler *errh) {
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    Vector<String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector
    uint32_t groupaddr = IPAddress(arg_list[0]);
    //iterate over the different group records
    bool found_group = false;
    for(int i = 0; i< element->group_records.size(); i++){
        //if the grouprecord has the same address as the input address then go into statement
        if (element->group_records[i].multicast_adress == groupaddr) {
            //The group address exists
            bool found_client = false;
            for (int y =0; y<element->group_records[i].sources.size(); y++){
                if (element->group_records[i].sources[y] == element->clientIP){
                    //the client's address has been found within the group record's source addresses
                    found_client = true;
                    element->group_records[i].mode = include;
                    WritablePacket * p =element->make_mem_report_packet();
                    //push the packet to update mode
                    element->output(0).push(p);
                    click_chatter("completed leave");
                }
            }
            if(!found_client){
                //the client does not exist in the group record
                click_chatter("did not find client when executing leave");
                return -1;
            }

        }
    }
    if(!found_group){
        //the group has not been found
        click_chatter("did not find group when executing leave");
        return -1;
    }
    return 0;
}

/*
 * function which initialises the handlers, current handlers contain:
 * -client_join which will let the client join the reception of a given multicast address /rfc page 4
 * -client leave which wil let the client leave the reception of a given multicast address /rfc page 4
 */
void IGMPClientSide::add_handlers() {
    click_chatter("hallo, adding handlers");

    add_write_handler("client_join", &client_join, (void*)0);
    add_write_handler("client_leave", &client_leave, (void*)0);
}

void IGMPClientSide::print_group_records(){
    for(int i = 0; i< group_records.size(); i++){
        click_chatter("group_record %i", i);
        click_chatter("\t multicast_adress %s", group_records[i].multicast_adress.unparse().c_str());
        click_chatter("\t record_type %s", group_records[i].record_type);
        click_chatter("\t number_of_sources %i", group_records[i].number_of_sources);
        click_chatter("\t mode %d", (int)group_records[i].mode);
        for(int y =0; y<group_records[i].sources.size(); y++){
            click_chatter("\t source %i %s",y, group_records[i].sources[y].unparse().c_str());
        }
    }
}


////////////////////////
/// PACKET FUNCTIONS ///
////////////////////////
/*
click_ip * IGMPClientSide::ip_encap(WritablePacket *p)
{
}

void* IGMPClientSide::igmp_encap(click_ip *p)
{
}
*/

uint32_t IGMPClientSide::_get_size_of_igmp_data()
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

void IGMPClientSide::_add_igmp_data(void *start)
{
    // add the membership report info
    igmp_mem_report *igmp_mr = reinterpret_cast<igmp_mem_report*>(start);
    igmp_mr->number_of_group_records = group_records.size();
    igmp_mr->type = IGMP_V3_MEM_RECORD;
    igmp_mr->checksum = 0; //TODO

    if (group_records.size() == 0) {return;};

    igmp_group_record_message *igmp_grp = (struct igmp_group_record_message*)(igmp_mr + 1);
    for(int i=0;i<group_records.size(); i++)
    {
        // set the fields in the reserved space to the correct thing
        igmp_grp->multicast_adress = group_records[i].multicast_adress.addr();
        igmp_grp->mode = group_records[i].mode;
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
    }
}

click_ip * IGMPClientSide::_add_ip_header(void *start)
{
    // based on elements/icmp/icmpsendpings.cc, line 133
    click_ip *nip = reinterpret_cast<click_ip*>(start); // place ip header at data pointer
    nip->ip_v = 4;
    nip->ip_hl = (sizeof(click_ip)) >> 2; //TODO ?
    nip->ip_len = htons(_get_size_of_igmp_data()); // TODO ?
    uint16_t ip_id = 1; // does not matter
    nip->ip_id = htons(ip_id); // converts host byte order to network byte order
    nip->ip_p = IP_PROTO_IGMP; // must be 2, check ipadress/clicknet/ip.h, line 56
    nip->ip_ttl = 1; // specified in RFC3376 page 7
    nip->ip_src = clientIP;
    nip->ip_dst = MC_ADDRESS; // all multicast routers listen to this adress
    nip->ip_sum = click_in_cksum((unsigned char *)nip, sizeof(click_ip)); // TODO misschien gewoon zo laten

    click_chatter("printing click ip while making report message: ");
    click_chatter("\t ip_v %d",nip->ip_v);
    click_chatter("\t ip_hl %d",nip->ip_hl);
    click_chatter("\t ip_len %d",nip->ip_len);
    click_chatter("\t ip_id %d",nip->ip_id);
    click_chatter("\t ip_p %d",nip->ip_p);
    click_chatter("\t ip_ttl %d",nip->ip_ttl);
//    click_chatter("\t ip_dst %s",nip->ip_dst.s_addr.c_str());
    click_chatter("\t ip_sum %d",nip->ip_sum);
    return nip;
}

// makes membership v3 packets, based on RFC3376 page 13-14
WritablePacket * IGMPClientSide::make_mem_report_packet()
{
    click_chatter("creating membership report packed");

    //uint32_t size = sizeof(click_ip) + sizeof(igmp_mem_report) + (sizeof(igmp_group_record_message)*group_records.size()); // TODO size of the entire packet
    WritablePacket *p = Packet::make( _get_size_of_igmp_data() + sizeof(click_ip));
    memset(p->data(), '\0', p->length()); // erase previous random data on memory requested

    click_ip *ip_header = _add_ip_header(p->data());
    _add_igmp_data(ip_header+1);

    // TODO ip otions
    p->set_ip_header(ip_header, sizeof(click_ip));
    p->timestamp_anno().assign_now();

    click_chatter("\t igmp data size: %i", _get_size_of_igmp_data());
    click_chatter("\t size of ip header: %i", sizeof(click_ip));
    click_chatter("\t total packet length %i", p->length());
    click_chatter("packet finished");
    return p;
}

void IGMPClientSide::push(int port, Packet *p)
{
    // TODO needs to accept and process queries (also something about udp)
    // unpacking data, based on elements/icmp/icmpsendpings.cc, line 194
    click_chatter("IGMP CLIENT %s recieved a packet in port %i ", clientIP.unparse().c_str(), port);
    const click_ip *iph = p->ip_header();

    // IGMP QUERIES
    if (port == 0)
    {
        // TODO
        return;
    }
    // UDP MESSAGES
    else if (port == 1)
    {
        if (p->has_network_header() and iph->ip_dst == clientIP)
        {
            output(1).push(p);
        }
        else
        {
            p->kill();
        }
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide) // forces to create element within click


