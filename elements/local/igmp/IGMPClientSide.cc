#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include "IGMPClientSide.hh"

CLICK_DECLS

IGMPClientSide::IGMPClientSide() : _timer(this){}

IGMPClientSide::~IGMPClientSide()
{
    delete helper;
}

/*

 * CLIENTADDRESS
 * MULTICASTADDRESS
 * ALLSYSTEMSMULTICASTADDRESS
 */
/**
 * configure for clientside object, should set the clientIP to the given clientaddress
 * might need to add some things later like timer variables
 * @param conf standard configure parameter, click practice
 * @param errh standard configure parameter, click practice
 * @return returns -1 on failed read and 0 on completed read
 */
int IGMPClientSide::configure(Vector <String> &conf, ErrorHandler *errh) {
    String ipadresstest;
    String madr;
    String asmadr;
    if (Args(conf, this, errh).read_mp("CADDR", clientIP)
    .read_mp("MADDR", MC_ADDRESS)
    .read_mp("ASMADDR",ALL_SYSTEMS_MC_ADDRESS)
    .read_mp("URI",unsolicited_report_interval)
    .read_mp("RV",robustness)
    .complete() <
        0) {
        click_chatter("failed read, returning -1", ipadresstest);
        return -1;
    }
    _timer.initialize(this);
    _timer.schedule_after_msec(1);
    //change to miliseconds for timer reasons
    unsolicited_report_interval= unsolicited_report_interval*1000;
    return 0;
}

/**
 * handles the client join for the system.
 * return integer, done throughout other click elements
 * @param conf standard handler parameter, click practice
 * @param e standard handler parameter, click practice
 * @param thunk standard handler parameter, click practice
 * @param errh standard handler parameter, click practice
 * @return returns integer -1 if the join failed or 0 if the join was completed
 */
int IGMPClientSide::client_join(const String &conf, Element *e, __attribute__((unused)) void *thunk,
                                __attribute__((unused)) ErrorHandler *errh) {
    //click_chatter("Entering join handler");
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    //click_chatter(element->clientIP.unparse().c_str());
    Vector <String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector

    uint32_t groupaddr = IPAddress(arg_list[0]).addr();

    bool exists = false;
    for (int i = 0; i < element->group_records.size(); i++) {
        if (element->group_records[i].multicast_adress == groupaddr) {
            //check if the client is a part of this group record already, might be helpful later
//            for (int y =0; y<element->group_records[i].sources.size(); y++){
//                if (element->group_records[i].sources[y] == element->clientIP){
//                    //case if the client has already joined the group address
//                    //the mc address exists and the client is already a part of it
//                    click_chatter("The group you are trying to join already has this source address");
//                    return -1;
//                }
//            }

            //if the client is not a part off the group, add it to the group, might be helpful later
//            element->group_records[i].sources.push_back(element->clientIP);
//            element->group_records[i].number_of_sources++;

            click_chatter("You have already joined this multicast group");
            return -1;
        }
    }

    //make new group record if it does not yet exist
    igmp_group_record grRecord;
    //the group record needs to be made
    grRecord.record_type = change_to_exclude;
    grRecord.multicast_adress = groupaddr;
    grRecord.number_of_sources = 0;
    element->group_records.push_back(grRecord);
    WritablePacket *p = element->make_mem_report_packet();
    URI_packages package;
    for(int i = 0; i<element->robustness-1;i++){
        package.timings.push_back(click_random(0, element->unsolicited_report_interval));
    }
    package.p = p;
    package._URI_timer =0;
    element->URI_messages.push_back(package);

//    element->output(0).push(p);
    click_chatter("client %s joined multicast group %s", element->clientIP.unparse().c_str(), IPAddress(groupaddr).unparse().c_str());
    //uncomment if you would like more information about the group records
    //element->print_group_records();
    return 0;
}


/**
 * handles the client leave, changes the filter mode fo that group record from exclude to include as defined in RFC
 * @param conf standard handler parameter, click practice
 * @param e standard handler parameter, click practice
 * @param thunk standard handler parameter, click practice
 * @param errh standard handler parameter, click practice
 * @return returns integer -1 if the join failed or 0 if the join was completed
 */
int IGMPClientSide::client_leave(const String &conf, Element *e, __attribute__((unused)) void *thunk,
                                 __attribute__((unused)) ErrorHandler *errh) {
    //click_chatter("Entering leave handler");
    IGMPClientSide *element = reinterpret_cast<IGMPClientSide *>(e); //convert element to igmpclientside element
    Vector <String> arg_list;
    cp_argvec(conf, arg_list);   //splits up the conf vector into more readable version in the arg_list vector
    uint32_t groupaddr = IPAddress(arg_list[0]).addr();
    //iterate over the different group records
    bool found_group = false;
    for (int i = 0; i < element->group_records.size(); i++) {
        //if the grouprecord has the same address as the input address then go into statement
        if (element->group_records[i].multicast_adress == groupaddr) {
            if (element->group_records[i].record_type != change_to_include) {
                //The group address exists
                found_group = true;
                element->group_records[i].record_type = change_to_include;
                WritablePacket *p = element->make_mem_report_packet();
                //push the packet to update mode
                URI_packages package;
                for(int i = 0; i<element->robustness-1;i++){
                    package.timings.push_back(click_random(0, element->unsolicited_report_interval));
                }
                package.p = p;
                package._URI_timer =0;
                element->URI_messages.push_back(package);

            } else {
                click_chatter("this client has already left this group");
                return -1;
            }
        }
    }
    if (!found_group) {
        //the group has not been found
        click_chatter("did not find group when executing leave");
        return -1;
    }
    return 0;
}


/**
 * function which initialises the handlers, current handlers contain:
 * -client_join which will let the client join the reception of a given multicast address /rfc page 4
 * -client leave which wil let the client leave the reception of a given multicast address /rfc page 4
 */
void IGMPClientSide::add_handlers() {
    //click_chatter("hallo, adding handlers");

    add_write_handler("join", &client_join, (void *) 0);
    add_write_handler("leave", &client_leave, (void *) 0);
}

/**
 * prints the group records, nice for debugging
 */
void IGMPClientSide::print_group_records() {
    for (int i = 0; i < group_records.size(); i++) {
        click_chatter("group_record %i", i);
        click_chatter("\t multicast_adress %s", group_records[i].multicast_adress.unparse().c_str());
        click_chatter("\t record_type %d", group_records[i].record_type);
        click_chatter("\t number_of_sources %i", group_records[i].number_of_sources);
        click_chatter("\t mode %d", (int) group_records[i].record_type);
        for (int y = 0; y < group_records[i].sources.size(); y++) {
            click_chatter("\t source %i %s", y, group_records[i].sources[y].unparse().c_str());
        }
    }
}


////////////////////////
/// PACKET FUNCTIONS ///
////////////////////////

/**
 * makes membership v3 packets, based on RFC3376 page 13-14
 * @return a writablepacket object which will be the IGMPV3 membership report
 */
WritablePacket *IGMPClientSide::make_mem_report_packet() {
    //click_chatter("creating membership report for %s", clientIP.unparse().c_str());

    //uint32_t size = sizeof(click_ip) + sizeof(igmp_mem_report) + (sizeof(igmp_group_record_message)*group_records.size()); // TODO size of the entire packet
    WritablePacket *p = Packet::make(helper->get_size_of_data(group_records) + sizeof(click_ip) + 4);
    memset(p->data(), 0, p->length()); // erase previous random data on memory requested

    click_ip *ip_header = helper->add_ip_header(p, clientIP, MC_ADDRESS);
    router_alert *r_alert = helper->add_router_alert(ip_header + 1);
    ip_header->ip_sum = click_in_cksum((unsigned char *) ip_header, sizeof(click_ip) + sizeof(router_alert));
    helper->add_igmp_data(r_alert + 1, group_records);

    p->set_ip_header(ip_header, sizeof(click_ip));
    p->timestamp_anno().assign_now();
    p->set_dst_ip_anno(IPAddress(MC_ADDRESS));
    return p;
}

/**
 * push function will get the packages and asses what needs to be done with them based on the port they entered
 * Port 0 will be IGMP Queries, these have not yet been implemented for deadline 1
 * Port 1 will be UDP messages, these are the messages multicasted by the router and accepted by the client if they have joined the
 * multicast group, we will also check whether the multicast group has the right filter mode so that the client only accepts
 * packets from the multicast group it has joined.
 * If the package is not an IGMP query or an UDP message, it is killed (RIP)
 * @param port the portnumber the packet has entered in
 * @param p The packet itself
 */
void IGMPClientSide::push(int port, Packet *p) {
    // TODO needs to accept and process queries (also something about udp)
    // unpacking data, based on elements/icmp/icmpsendpings.cc, line 194

    const click_ip *ip_header = p->ip_header();
    click_chatter("received a package with protocol number %d", ip_header->ip_p);
    if (ip_header->ip_p == IP_PROTO_IGMP) {
    // IGMP QUERIES
        click_chatter("the client has received a igmp query");
        //ontleed de query, check van waar ze komt.
//        igmp_mem_query_msg* query = (igmp_mem_query_msg*) ()
        for (int i = 0; i<group_records.size(); i++){
            //als group record mc adr overeenkomt met src van query, stuur v3 als join
            if(group_records[i].record_type == 4 or group_records[i].record_type == 2){
                group_records[i].record_type = 2;
//                igmp_group_record grRecord;
//                //the group record needs to be made
//                grRecord.record_type = change_to_exclude;
//                grRecord.multicast_adress = groupaddr;
//                grRecord.number_of_sources = 0;

                WritablePacket *p = make_mem_report_packet();
                click_chatter("Sending a package back to the router filter mode exclude");
                output(0).push(p);

            }
        }
        return;
    }
    // UDP MESSAGES
    else if (ip_header->ip_p == 17) {
        if (p->has_network_header()) {
            for (int i = 0; i < group_records.size(); i++) {
                if (group_records[i].multicast_adress == ip_header->ip_dst and group_records[i].record_type == 4) {
                    //click_chatter("client %s recieved their packet", clientIP.unparse().c_str());
                    output(1).push(p);
                }
            }
        } else {
            p->kill();
        }
    }
    else
    {
        output(port).push(p);
    }
}

/**
 * run timer will be triggered after the _timer gets scheduled
 * the timer gets scheduled by using _timer.schedule_after_sec(time)
 * you can check whether the timer is set by using _timer.scheduled()
 */
void IGMPClientSide::run_timer(Timer * timer)
{

    assert(timer == &_timer);
    _timer.schedule_after_msec(1);
    if(URI_messages.size() > 0){
        URI_packages current_message = URI_messages[0];

        if(URI_messages[0].timings.size() > 0){
            if(URI_messages[0].timings[0] == _local_timer){
                click_chatter("the timing size is %d", URI_messages[0].timings.size());

                Packet *package = URI_messages[0].p->clone();
                output(0).push(package);
//                for(int b = 0; b<current_message.timings.size(); b++){
//                }
                URI_messages[0].timings.erase(URI_messages[0].timings.begin());
                _local_timer = 0;
                click_chatter("post timing size is %d", URI_messages[0].timings.size());

            }
        }
        else{
            URI_messages.erase(URI_messages.begin());

        }
        if(URI_messages[0].timings.size() ==0){
            URI_messages.erase(URI_messages.begin());

        }
        _local_timer++;

    }

//    click_chatter("timer tick %d", _local_timer);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientSide) // forces to create element within click


