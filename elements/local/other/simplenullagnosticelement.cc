#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "simplenullagnosticelement.hh"

CLICK_DECLS
SimpleNullAgnosticElement::SimpleNullAgnosticElement()
{}

SimpleNullAgnosticElement::~ SimpleNullAgnosticElement()
{}

int SimpleNullAgnosticElement::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (conf.size() > 0) return errh->error("oversized configuration string");
    return 0;
}

void SimpleNullAgnosticElement::push(int, Packet *p){
	output(0).push(p);
}

Packet* SimpleNullAgnosticElement::pull(int){
	return input(0).pull();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SimpleNullAgnosticElement)
