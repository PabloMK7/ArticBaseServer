#include "hidExtension.hpp"
#include <string.h>
#include <3ds/types.h>
#include <3ds/result.h>
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/synchronization.h>
#include <3ds/services/fs.h>
#include <3ds/ipc.h>
#include <3ds/env.h>

extern Handle hidHandle;

Result HIDUSER_GetGyroscopeCalibrateParam(GyroscopeCalibrateParam* calibrateParam) {
    u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x16,0,0);

	Result ret = 0;
	if(R_FAILED(ret = svcSendSyncRequest(hidHandle))) return ret;

	if (R_SUCCEEDED(cmdbuf[1])) {
        memcpy(calibrateParam, &cmdbuf[2], sizeof(GyroscopeCalibrateParam));
    }
    return cmdbuf[1];
}