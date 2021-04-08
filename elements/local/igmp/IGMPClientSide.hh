#ifndef CLICK_IGMPCLIENTSIDE_HH
#define CLICK_IGMPCLIENTSIDE_HH
#include <click/element.hh>
CLICK_DECLS

class IGMPClientSide : public Element {
public:
    IGMPClientSide();
    ~IGMPClientSide();

    const char *class_name() const	{ return "IGMPClientSide"; }
    const char *port_count() const	{ return "1/1"; }
    const char *processing() const	{ return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

//    void push(int, Packet *);

    //static handles, important will not work otherwise
    static int client_join(const String &conf, Element *e, void *thunk, ErrorHandler *errh);
    void add_handlers();

private:
    uint32_t maxSize;
    Vector<Vector<click_ip>> groups;
    Packet* make_membership_packet();
};

CLICK_ENDDECLS
#endif

