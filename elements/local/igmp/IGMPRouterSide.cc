//
// Created by Gebruiker on 7/04/2021.
//
#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/ipaddress.hh>
#include <clicknet/ip.h>
#include "IGMPRouterSide.hh"

CLICK_DECLS
IGMPRouterSide::IGMPRouterSide() : _timer(this){}

IGMPRouterSide::~IGMPRouterSide() {}


/**
 * configures the routerside object, only takes therouter ip or interface as input
 * @param conf standard configure parameter, click practice
 * @param errh standard configure parameter, click practice
 * @return returns -1 on failed read and 0 on completed read
 */
int IGMPRouterSide::configure(Vector <String> &conf, ErrorHandler *errh) {
    //cunfugure like RFC 8.14
    if (Args(conf, this, errh)
    .read_mp("ROUTERADDRESS", routerIP)
    .read_mp("RV", robustness_variable)
    .read_mp("QI", query_interval)
    .read_mp("SQI", startup_interval)
    .read_mp("SQC", startup_count)
    .read_mp("LMQI", LMQI).read_mp("LMQC", LMQC)
    .read_mp("MRT", max_response_time).complete() < 0) {
        click_chatter("failed read when initialising router, returning 0");
        if(0<robustness_variable<7){
            return  -1;
        }
        return -1;
    }
    GMI = (robustness_variable * query_interval) + max_response_time;
    LMQT = LMQI * LMQC;

    for (int i=0; i < port_count()[2] - 48; i++)
    {
        interface_states.push_back(Vector<igmp_group_state>());
    }

    // initialize group timer
    _timer.initialize(this);
    _timer.schedule_after_sec(0);

    // initialize general query timer
    query_timer* qt = new query_timer;
    qt->router = this;
    Timer* general_timer= new Timer(&general_query_timer, qt);
    general_timer->initialize(this);
    general_timer->schedule_after_msec(0);
    return 0;
}

/***
 * processing of reports based on their type
 * @param record: the group record report
 * @param port: port where it was recieved
 */
void IGMPRouterSide::process_report(igmp_group_record *record, int port)
{
    if (record->record_type == IGMP_V3_EXCLUDE) {
        // source lists will always be empty so nly need to update the group timer
        for (igmp_group_state &state: interface_states[port])
        {
            if (state.multicast_adress == record->multicast_adress) {state.group_timer = GMI;}
        }
    }
    else if (record->record_type == IGMP_V3_CHANGE_TO_INCLUDE) // leaves
    {
        // TODO queries sent need to be transmitted [Last Member Query Count] times, once every [Last Member Query Interval] (page 32)
        Packet *q = make_group_specific_query_packet(record->multicast_adress, port);
        output(port + 6).push(q);

    }
    else if (record->record_type == IGMP_V3_CHANGE_TO_EXCLUDE) // joins
    {
        bool exists = false;
        for (igmp_group_state &state: interface_states[port])
        {
            if (record->multicast_adress == state.multicast_adress)
            {
                // there is interest in the group, so we set the type back to exclude, and update the group timer
                exists = true;
                state.mode = IGMP_V3_EXCLUDE;
                state.group_timer = GMI;
            }
        }

        if (!exists)
        {
            igmp_group_state new_state;
            new_state.mode = IGMP_V3_EXCLUDE;
            new_state.group_timer = GMI;
            new_state.multicast_adress = record->multicast_adress;
            interface_states[port].push_back(new_state);
            new_state.source_records = Vector<igmp_source_record>();
        }
    }
}

/***
 * a timer for updating the group timers
 * @param timer
 */
void IGMPRouterSide::run_timer(Timer * timer)
{
    _timer.schedule_after_msec(100);

    // updating group timers
    for (auto &interface: interface_states)
    {
        // update group timers
        for (igmp_group_state &state: interface)
        {
            //click_chatter("group timer = %i", state.group_timer);
            if (state.group_timer < 1)
            {
                // group timer ran out, transition to include mode
                state.mode = IGMP_V3_INCLUDE; //TODO wanneer deleten, of nooit delete? momenteel probleem dat dubbel joinen niet werkt
            }
            else
            {
                state.group_timer -= 1;
            }
        }
    }
}

/***
 * a timer for sending general queries, both during startup and after
 * @param timer
 * @param data
 */
void IGMPRouterSide::general_query_timer(Timer * timer, void* data){

    query_timer* timerdata = (query_timer*) data;
    Packet *q = timerdata->router->make_general_query_packet();
    for(int i = 6; i<9;i++){
        Packet *package = q->clone();
        timerdata->router->output(i).push(package);
    }
    if (timerdata->router->startup_count > 1)
    {
        timerdata->router->startup_count--;
        timer->schedule_after_msec(timerdata->router->startup_interval * 100);
    }
    else
    {
        timer->schedule_after_msec(timerdata->router->query_interval * 100);
    }
}

/***
 * makes a general query
 * @return the query packet
 */
WritablePacket * IGMPRouterSide::make_general_query_packet()
{
    WritablePacket *p = Packet::make(query_helper->get_size_of_data(0) + sizeof(click_ip) + 4);
    memset(p->data(), 0, p->length()); // erase previous random data on memory requested

    click_ip *ip_header = query_helper->add_ip_header(p, routerIP, ASMC_ADDRESS, false);
    router_alert *r_alert = query_helper->add_router_alert(ip_header + 1);
    ip_header->ip_sum = click_in_cksum((unsigned char *) ip_header, sizeof(click_ip) + sizeof(router_alert));
    query_helper->add_igmp_data(r_alert + 1, Vector <IPAddress>(), IPAddress("0.0.0.0"), false,
                                robustness_variable, query_interval / 10, max_response_time);

    p->set_ip_header(ip_header, sizeof(click_ip));
    p->timestamp_anno().assign_now();
    p->set_dst_ip_anno(IPAddress(ASMC_ADDRESS));
    return p;
}

/***
 * makes a group specific query
 * @return the query packet
 */
WritablePacket * IGMPRouterSide::make_group_specific_query_packet(IPAddress group_address, int interface)
{
    for (igmp_group_state &state: interface_states[interface]) {
        if (state.multicast_adress == group_address) {
            // when querying a specific group, lower that groups timer to a small interval
            state.group_timer = LMQT;// TODO wat is da small interval

            // make the packet
            WritablePacket *p = Packet::make(query_helper->get_size_of_data(0) + sizeof(click_ip) + 4);
            memset(p->data(), 0, p->length()); // erase previous random data on memory requested

            click_ip *ip_header = query_helper->add_ip_header(p, routerIP, ASMC_ADDRESS,false);
            router_alert *r_alert = query_helper->add_router_alert(ip_header + 1);
            ip_header->ip_sum = click_in_cksum((unsigned char *) ip_header, sizeof(click_ip) + sizeof(router_alert));

            query_helper->add_igmp_data(r_alert + 1, Vector<IPAddress>(), state.multicast_adress, true, robustness_variable, query_interval / 10, max_response_time);

            p->set_ip_header(ip_header, sizeof(click_ip));
            p->timestamp_anno().assign_now();
            p->set_dst_ip_anno(IPAddress(ASMC_ADDRESS));
            return p;
        }
    }
    click_chatter("did not find group when sending group specific query");
    return NULL;
}

/**
 * router push function, router can get mulitple types of packets, right now we only look at UDP and igmp v3 membership reports
 * when obtaining an igmp message, the router will add this client to its group state if it was not there already, the packet will be unpacked
 * based on the packets content the router will see if the client want to join or leave. Router will alter group state accordingly
 * if the router gets an UDP packet, it will multicast this to all clients interested.
 * Other messages get pushed to another output port
 * @param port input port of the packet
 * @param p the packet itself
 */
void IGMPRouterSide::push(int port, Packet *p) {

    const click_ip *ip_header = p->ip_header();
    if (ip_header->ip_p == IP_PROTO_IGMP) {
        // unpacking data
        const router_alert *alert_ptr = reinterpret_cast<const router_alert *>(ip_header + 1);
        const igmp_mem_report *info_ptr = reinterpret_cast<const igmp_mem_report *>(alert_ptr + 1);
        const igmp_group_record_message *records_ptr = reinterpret_cast<const igmp_group_record_message *>(info_ptr +1);
        igmp_mem_report report_info = report_helper->igmp_unpack_info(info_ptr);
        Vector <igmp_group_record> group_records = report_helper->igmp_unpack_group_records(records_ptr, report_info.number_of_group_records);

        // processing packet
        for (int i = 0; i < group_records.size(); i++)
        {
            uint8_t record_type = group_records[i].record_type;
            process_report(&group_records[i], port);
        }
    }
    else if (ip_header->ip_p == 17)
    {
        multicast_udp_packet(p);
    }
    else
    {
        output(port).push(p);
    }
}

/**
 * forward packets to interfacest that are interested
 * @param p the packet to multicast
 * @param port the input port
 */
void IGMPRouterSide::multicast_udp_packet(Packet *p) {
    const click_ip *ip_header = p->ip_header();
    //click_chatter("hlen, %i", length);
    for (int i = 0; i < interface_states.size(); i++) {
        for (int j = 0; j < interface_states[i].size(); j++) {
            igmp_group_state &state = interface_states[i][j];
            if (state.multicast_adress == ip_header->ip_dst) {
                // check if the source adress is in the source records
                int index = -1;
                for (int k=0; i < state.source_records.size(); i++)
                {
                    if (state.source_records[k].source_address == ip_header->ip_src)
                    {
                        index = k;
                    }
                }

                // if the group timer is zero, and the mode is include
                if (state.group_timer == 0 and state.mode == IGMP_V3_INCLUDE)
                {
                    // delete the source record
                    if (index != -1)
                    {
                        state.source_records.erase(state.source_records.begin() + index);
                    }
                    // if no more source records exist, delete the group record
                    if (state.source_records.size() == 0)
                    {
                        // click_chatter("removing group state");
                        interface_states[i].erase(interface_states[i].begin() + j);
                    }
                }

                // send the packet if the group state is include and it is in the source records, or the reverse for exclude
                if (((IGMP_V3_INCLUDE == state.mode) == (index != -1)) and state.group_timer > 0)
                {
                    Packet *package = p->clone();
                    output(i + 3).push(package);
                }
            }
        }
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterSide) // forces to create element within click
