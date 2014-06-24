// -*- c++ -*-
#ifndef PING_STATE_LISTENER_H
#define PING_STATE_LISTENER_H

class StateListener {
public:
    virtual void onBounce() {}
    virtual void onHit() {}
};

#endif
