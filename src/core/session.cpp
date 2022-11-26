/*!
* \brief session.
*/

#include "dlas/dlas.hpp"
#include <iostream>

namespace dlas {

struct SessionParams {
    std::string name;
    SessionMode mode;
};

Session::Session(const std::string& name, SessionMode mode) {
    SessionParams *p = new SessionParams;
    p->name = name;
    p->mode = mode;

    params_ = (void *)p;
}

Session::~Session() {
    delete (SessionParams *)params_;
}

void Session::Run() {
    SessionParams *p = (SessionParams *)params_;
    printf("Session Running: %s, %d.\n", p->name.c_str(), p->mode);
}

} // dlas.