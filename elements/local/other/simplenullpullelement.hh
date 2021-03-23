#ifndef CLICK_SIMPLENULLPULLELEMENT_HH
#define CLICK_SIMPLENULLPULLELEMENT_HH
#include <click/element.hh>
CLICK_DECLS

class SimpleNullPullElement : public Element {
	public:
        SimpleNullPullElement();
		~SimpleNullPullElement();
		
		const char *class_name() const	{ return "SimpleNullPullElement"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*);
		
		Packet* pull(int);
	private:
		uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
