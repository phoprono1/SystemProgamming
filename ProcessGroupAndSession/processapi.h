#ifndef PROCESSAPI_H
#define PROCESSAPI_H

#include <sys/types.h>

class ProcessAPI {
public:
    static pid_t getSID();
    static pid_t getPGID();
    static pid_t getPID();
};

#endif // PROCESSAPI_H
