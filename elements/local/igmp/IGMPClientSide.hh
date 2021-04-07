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

    void push(int, Packet *);
private:
    uint32_t maxSize;
};

CLICK_ENDDECLS
#endif

