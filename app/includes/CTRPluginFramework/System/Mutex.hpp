#ifndef CTRPLUGINFRAMEWORK_SYSTEM_MUTEX_HPP
#define CTRPLUGINFRAMEWORK_SYSTEM_MUTEX_HPP

#include "3ds.h"

namespace CTRPluginFramework
{
    class Mutex
    {
    public:
        inline Mutex(void) {
            RecursiveLock_Init(&_lock);
        }
        inline ~Mutex(void) {
            // I suppose that we can "force" unlock the mutex
            if (_lock.counter > 0)
            {
                _lock.counter = 1;
                RecursiveLock_Unlock(&_lock);
            }
        }

        inline void    Lock(void) {
            RecursiveLock_Lock(&_lock);
        }

        // Return true on failure
        inline bool    TryLock(void) {
            return RecursiveLock_TryLock(&_lock) != 0;
        }
        
        inline void    Unlock(void) {
            RecursiveLock_Unlock(&_lock);
        }

    private:
        RecursiveLock _lock;
    };
}

#endif
