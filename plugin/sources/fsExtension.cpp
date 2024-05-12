#include "fsExtension.hpp"
#include <string.h>
#include <3ds/types.h>
#include <3ds/result.h>
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/synchronization.h>
#include <3ds/services/fs.h>
#include <3ds/ipc.h>
#include <3ds/env.h>

Result FSUSER_NewSetSaveDataSecureValue(FS_Archive archive, u64 value, FS_SecureValueSlot slot, bool flush)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x875,6,0);
    cmdbuf[1] = (u32) archive;
	cmdbuf[2] = (u32) (archive >> 32);
	cmdbuf[3] = slot;
	cmdbuf[4] = (u32) value;
	cmdbuf[5] = (u32) (value >> 32);
	cmdbuf[6] = flush;

	Result ret = 0;
	if(R_FAILED(ret = svcSendSyncRequest(*fsGetSessionHandle()))) return ret;

	return cmdbuf[1];
}

Result FSUSER_NewGetSaveDataSecureValue(bool* exists, bool* isGamecard, u64* value, FS_Archive archive, FS_SecureValueSlot slot)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x876,3,0);
    cmdbuf[1] = (u32) archive;
	cmdbuf[2] = (u32) (archive >> 32);
	cmdbuf[3] = slot;

	Result ret = 0;
	if(R_FAILED(ret = svcSendSyncRequest(*fsGetSessionHandle()))) return ret;

    if (R_SUCCEEDED(cmdbuf[1])) {
        if(exists) *exists = cmdbuf[2] & 0xFF;
        if(isGamecard) *isGamecard = cmdbuf[3] & 0xFF;
        if(value) *value = cmdbuf[4] | ((u64) cmdbuf[5] << 32);
    }

	return cmdbuf[1];
}


Result FSUSER_SetThisSaveDataSecureValue(u64 value, FS_SecureValueSlot slot)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x86E,3,0);
	cmdbuf[1] = slot;
	cmdbuf[2] = (u32) value;
	cmdbuf[3] = (u32) (value >> 32);

	Result ret = 0;
	if(R_FAILED(ret = svcSendSyncRequest(*fsGetSessionHandle()))) return ret;

	return cmdbuf[1];
}

Result FSUSER_GetThisSaveDataSecureValue(bool* exists, bool* isGamecard, u64* value, FS_SecureValueSlot slot)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x86F,1,0);
	cmdbuf[1] = slot;

	Result ret = 0;
	if(R_FAILED(ret = svcSendSyncRequest(*fsGetSessionHandle()))) return ret;

    if (R_SUCCEEDED(cmdbuf[1])) {
        if(exists) *exists = cmdbuf[2] & 0xFF;
        if(isGamecard) *isGamecard = cmdbuf[3] & 0xFF;
        if(value) *value = cmdbuf[4] | ((u64) cmdbuf[5] << 32);
    }

	return cmdbuf[1];
}
