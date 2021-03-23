#ifndef CLICK_SIMPLENULLPUSHELEMENT_HH
#define CLICK_SIMPLENULLPUSHELEMENT_HH
#include <click/element.hh>
CLICK_DECLS

class SimpleNullPushElement : public Element {
	public:
        SimpleNullPushElement();
		~SimpleNullPushElement();
		
		const char *class_name() const	{ return "SimpleNullPushElement"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
