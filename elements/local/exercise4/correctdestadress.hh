#ifndef CLICK_CORRECTDESTADRESS_HH
#define CLICK_CORRECTDESTADRESS_HH
#include <click/element.hh>
CLICK_DECLS

class CorrectDestAdress : public Element {
    struct in_addr _my_ip;
public:
        CorrectDestAdress();
        ~CorrectDestAdress();

        const char *class_name() const	{ return "CorrectDestAdress"; }
        const char *port_count() const	{ return "1/1"; }
        const char *processing() const	{ return PUSH; }
        int configure(Vector<String>&, ErrorHandler*);

        void push(int, Packet *);
    private:
        uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
