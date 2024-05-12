#include <3ds.h>
#include "plgldr.h"
#include "csvc.h"

static Handle   plgLdrHandle = 0;
static int      plgLdrRefCount;

Result  plgLdrInit(void)
{
    Result res = 0;

    if (AtomicPostIncrement(&plgLdrRefCount) == 0)
        res = svcConnectToPort(&plgLdrHandle, "plg:ldr");

    if (R_FAILED(res)) AtomicDecrement(&plgLdrRefCount);

    return res;
}

void    plgLdrExit(void)
{
    if (AtomicDecrement(&plgLdrRefCount))
        return;
    svcCloseHandle(plgLdrHandle);
}

Result  PLGLDR__IsPluginLoaderEnabled(bool *isEnabled)
{
    Result res = 0;

    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(2, 0, 0);
    if (R_SUCCEEDED((res = svcSendSyncRequest(plgLdrHandle))))
    {
        res = cmdbuf[1];
        *isEnabled = cmdbuf[2];
    }
    return res;
}

Result  PLGLDR__SetPluginLoaderState(bool enabled)
{
    Result res = 0;

    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(3, 1, 0);
    cmdbuf[1] = (u32)enabled;

    if (R_SUCCEEDED((res = svcSendSyncRequest(plgLdrHandle))))
    {
        res = cmdbuf[1];
    }
    return res;
}

Result  PLGLDR__SetPluginLoadParameters(PluginLoadParameters *parameters)
{
    Result res = 0;

    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(4, 2, 4);
    cmdbuf[1] = (u32)parameters->noFlash | (((u32)parameters->pluginMemoryStrategy) << 8) | (((u32)parameters->persistent) << 16);
    cmdbuf[2] = parameters->lowTitleId;
    cmdbuf[3] = IPC_Desc_Buffer(256, IPC_BUFFER_R);
    cmdbuf[4] = (u32)parameters->path;
    cmdbuf[5] = IPC_Desc_Buffer(32 * sizeof(u32), IPC_BUFFER_R);
    cmdbuf[6] = (u32)parameters->config;

    if (R_SUCCEEDED((res = svcSendSyncRequest(plgLdrHandle))))
    {
        res = cmdbuf[1];
    }
    return res;
}

Result  PLGLDR__GetVersion(u32* version)
{
	Result res = 0;

	u32* cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(8, 0, 0);

	if (R_SUCCEEDED((res = svcSendSyncRequest(plgLdrHandle))))
	{
		if (cmdbuf[0] != IPC_MakeHeader(8, 2, 0))
			return 0xD900182F;

		res = cmdbuf[1];
		if (version)
			*version = cmdbuf[2];
	}
	return res;
}

Result  PLGLDR__SetExeDecSettings(void* decFunc, void* args)
{
	Result res = 0;

	u32 buf[0x10] = { 0 };
	u32* trueArgs;
	if (args) trueArgs = args;
	else trueArgs = buf;

	u32* cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(13, 1, 2);
	cmdbuf[1] = (decFunc) ? svcConvertVAToPA(decFunc, false) | (1 << 31) : 0;
	cmdbuf[2] = IPC_Desc_Buffer(16 * sizeof(u32), IPC_BUFFER_R);
	cmdbuf[3] = (u32)trueArgs;

	if (R_SUCCEEDED((res = svcSendSyncRequest(plgLdrHandle))))
	{
		res = cmdbuf[1];
	}
	return res;
}

Result  PLGLDR__ClearPluginLoadParameters()
{
    Result res = 0;

    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(14, 0, 0);
    if (R_SUCCEEDED((res = svcSendSyncRequest(plgLdrHandle))))
    {
        res = cmdbuf[1];
    }
    return res;
}