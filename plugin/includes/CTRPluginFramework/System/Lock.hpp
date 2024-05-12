#ifndef CTRPLUGINFRAMEWORK_SYSTEM_LOCK_HPP
#define CTRPLUGINFRAMEWORK_SYSTEM_LOCK_HPP

#include "3ds.h"

namespace CTRPluginFramework
{
    class Mutex;
    class Lock
    {
    public:
        inline explicit Lock(LightLock &llock) :
            _type{LIGHTLOCK}, _llock{&llock}
        {
            LightLock_Lock(_llock);
        }

        inline explicit Lock(RecursiveLock &rlock) :
            _type{RECLOCK}, _rlock{&rlock}
        {
            RecursiveLock_Lock(_rlock);
        }

        inline explicit Lock(Mutex &mutex) :
            _type{MUTEX}, _mutex{&mutex}
        {
            mutex.Lock();
        }

        inline ~Lock(void)
        {
            if (_type == LIGHTLOCK)
                LightLock_Unlock(_llock);
            else if (_type == RECLOCK)
                RecursiveLock_Unlock(_rlock);
            else if (_type == MUTEX)
                _mutex->Unlock();
        }

    private:
        static const constexpr u32 LIGHTLOCK = 1;
        static const constexpr u32 RECLOCK   = 2;
        static const constexpr u32 MUTEX     = 3;
        const u32     _type;
        union
        {
            LightLock       *_llock;
            RecursiveLock   *_rlock;
            Mutex           *_mutex;
        };
    };
}

#endif
