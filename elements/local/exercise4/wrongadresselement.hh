#ifndef CLICK_WRONGADRESSELEMENT_HH
#define CLICK_WRONGADRESSELEMENT_HH
#include <click/element.hh>
CLICK_DECLS

class WrongAdressElement : public Element {
    struct in_addr _my_ip;
public:
        WrongAdressElement();
        ~WrongAdressElement();

        const char *class_name() const	{ return "WrongAdressElement"; }
        const char *port_count() const	{ return "1/1"; }
        const char *processing() const	{ return PUSH; }
        int configure(Vector<String>&, ErrorHandler*);

        void push(int, Packet *);
    private:
        uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
