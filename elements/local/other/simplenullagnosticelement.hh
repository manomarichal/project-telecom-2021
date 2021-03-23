#ifndef CLICK_SIMPLENULLAGNOSTICELEMENT_HH
#define CLICK_SIMPLENULLAGNOSTICELEMENT_HH
#include <click/element.hh>
CLICK_DECLS

class SimpleNullAgnosticElement : public Element {
	public:
        SimpleNullAgnosticElement();
		~SimpleNullAgnosticElement();
		
		const char *class_name() const	{ return "SimpleNullAgnosticElement"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return AGNOSTIC; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
		Packet* pull(int);
	private:
		uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
