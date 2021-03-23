#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "simplenullpushelement.hh"

CLICK_DECLS
SimpleNullPushElement::SimpleNullPushElement()
{}

SimpleNullPushElement::~ SimpleNullPushElement()
{}

int SimpleNullPushElement::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (conf.size() > 0) return errh->error("oversized configuration string");
    return 0;
}

void SimpleNullPushElement::push(int, Packet *p){
	return output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SimpleNullPushElement)
