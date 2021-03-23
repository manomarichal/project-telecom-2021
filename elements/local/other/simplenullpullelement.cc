#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "simplenullpullelement.hh"

CLICK_DECLS
SimpleNullPullElement::SimpleNullPullElement()
{}

SimpleNullPullElement::~ SimpleNullPullElement()
{}

int SimpleNullPullElement::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (conf.size() > 0) return errh->error("oversized configuration string");
    return 0;
}

Packet* SimpleNullPullElement::pull(int){
	return input(0).pull();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SimpleNullPullElement)
