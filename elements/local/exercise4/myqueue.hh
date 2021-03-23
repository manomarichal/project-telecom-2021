#ifndef CLICK_MYQUEUE_HH
#define CLICK_MYQUEUE_HH
#include <click/element.hh>
#include <queue>
CLICK_DECLS

class MyQueue : public Element {
    struct in_addr _my_ip;
public:
        MyQueue();
        ~MyQueue();

        const char *class_name() const	{ return "MyQueue"; }
        const char *port_count() const	{ return "1/2"; }
        const char *processing() const	{ return PUSH_TO_PULL;}
        int configure(Vector<String>&, ErrorHandler*);

    void push(int, Packet *);
    Packet* pull(int);
    private:
        std::queue<Packet*> queue;
    std::queue<Packet*> discarded;
        uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
