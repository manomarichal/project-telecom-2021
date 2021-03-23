#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "myqueue.hh"
#include <iostream>
#include <queue>
CLICK_DECLS
MyQueue::MyQueue()
{
}

MyQueue::~ MyQueue()
{}

int MyQueue::configure(Vector<String> &conf, ErrorHandler *errh) {
    return 0;
}

void MyQueue::push(int port, Packet *p_in){
    if (port == 0)
    {
        if (queue.size() < maxSize)
        {
            queue.push(p_in);
        }
        else
        {
            discarded.push(p_in);
        }
    }
}

Packet * MyQueue::pull(int port)
{
    if (port == 0)
        if (queue.empty())
        {
            return 0;
        }
        else
        {

            Packet* p = queue.front();
            queue.pop();
            return p;
        }
    else
    {
        if (discarded.empty())
        {
            return 0;
        }
        else
        {

            Packet* p = discarded.front();
            discarded.pop();
            return p;
        }
    }

}

CLICK_ENDDECLS
EXPORT_ELEMENT(MyQueue)

