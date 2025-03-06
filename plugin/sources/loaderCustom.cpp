#include "3ds.h"

namespace ArticFunctions {

    static Handle loaderHandleCustom;
    static int loaderRefCountCustom;
    Result loaderInitCustom(void)
    {
        Result res;
        if (AtomicPostIncrement(&loaderRefCountCustom)) return 0;
        res = srvGetServiceHandle(&loaderHandleCustom, "Loader");
        if (R_FAILED(res)) AtomicDecrement(&loaderRefCountCustom);
        return res;
    }

    void loaderExitCustom(void)
    {
        if (AtomicDecrement(&loaderRefCountCustom)) return;
        svcCloseHandle(loaderHandleCustom);
    }

    Result LOADER_GetLastApplicationProgramInfo(ExHeader_Info* exheaderInfo)
    {
        Result ret = 0;
        u32 *cmdbuf = getThreadCommandBuffer();
        u32 *staticbufs = getThreadStaticBuffers();

        cmdbuf[0] = IPC_MakeHeader(0x102, 0, 0);

        u32 staticbufscpy[2] = {staticbufs[0], staticbufs[1]};
        staticbufs[0] = IPC_Desc_StaticBuffer(0x400, 0);
        staticbufs[1] = (u32)exheaderInfo;

        if(R_FAILED(ret = svcSendSyncRequest(loaderHandleCustom))) return ret;

        staticbufs[0] = staticbufscpy[0];
        staticbufs[1] = staticbufscpy[1];

        return (Result)cmdbuf[1];
    }
}
