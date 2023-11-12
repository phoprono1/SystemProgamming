#include "processapi.h"
#include <unistd.h>

pid_t ProcessAPI::getSID() {
    return getsid(0);
}

pid_t ProcessAPI::getPGID() {
    return getpgrp();
}

pid_t ProcessAPI::getPID() {
    return getpid();
}
