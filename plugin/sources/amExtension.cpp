#include "3ds.h"
#include <3ds/result.h>
#include <3ds/svc.h>
#include "amExtension.hpp"

Result AM_GetTitleInfoIgnorePlatform(FS_MediaType mediatype, u32 titleCount, u64 *titleIds, AM_TitleEntry *titleInfo)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x2C,2,4); // 0x002C0084
	cmdbuf[1] = mediatype;
	cmdbuf[2] = titleCount;
	cmdbuf[3] = IPC_Desc_Buffer(titleCount*sizeof(u64),IPC_BUFFER_R);
	cmdbuf[4] = (u32)titleIds;
	cmdbuf[5] = IPC_Desc_Buffer(titleCount*sizeof(AM_TitleEntry),IPC_BUFFER_W);
	cmdbuf[6] = (u32)titleInfo;

	if(R_FAILED(ret = svcSendSyncRequest(*amGetSessionHandle()))) return ret;

	return (Result)cmdbuf[1];
}

Result AMAPP_FindDLCContentInfos(FS_MediaType mediatype, u64 title_id, u32 contentCount, u16 *contentIds, AM_ContentInfo *contentInfos)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x1002,4,4); // 0x10020104
	cmdbuf[1] = mediatype;
	*(u64*)(&cmdbuf[2]) = title_id;
    cmdbuf[4] = contentCount;
	cmdbuf[5] = IPC_Desc_Buffer(contentCount*sizeof(u16),IPC_BUFFER_R);
	cmdbuf[6] = (u32)contentIds;
	cmdbuf[7] = IPC_Desc_Buffer(contentCount*sizeof(AM_ContentInfo),IPC_BUFFER_W);
	cmdbuf[8] = (u32)contentInfos;

	if(R_FAILED(ret = svcSendSyncRequest(*amGetSessionHandle()))) return ret;

	return (Result)cmdbuf[1];
}

Result AMAPP_GetDLCTitleInfos(FS_MediaType mediatype, u32 titleCount, u64 *titleIds, AM_TitleEntry *titleInfo)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x1005,2,4); // 0x10050084
	cmdbuf[1] = mediatype;
	cmdbuf[2] = titleCount;
	cmdbuf[3] = IPC_Desc_Buffer(titleCount*sizeof(u64),IPC_BUFFER_R);
	cmdbuf[4] = (u32)titleIds;
	cmdbuf[5] = IPC_Desc_Buffer(titleCount*sizeof(AM_TitleEntry),IPC_BUFFER_W);
	cmdbuf[6] = (u32)titleInfo;

	if(R_FAILED(ret = svcSendSyncRequest(*amGetSessionHandle()))) return ret;

	return (Result)cmdbuf[1];
}

Result AMAPP_ListDataTitleTicketInfos(u32* ticketReadCount, u64 titleID, u32 ticketInfoCount, u32 offset, AM_TicketInfo* ticketInfos)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = (IPC_MakeHeader(0x1007,4,2)); // 0x10070102
	cmdbuf[1] = ticketInfoCount;
	cmdbuf[2] = titleID & 0xffffffff;
	cmdbuf[3] = (u32)(titleID >> 32);
	cmdbuf[4] = offset;
	cmdbuf[5] = IPC_Desc_Buffer(ticketInfoCount * sizeof(AM_TicketInfo), IPC_BUFFER_W);
	cmdbuf[6] = (u32)ticketInfos;

	if(R_FAILED(ret = svcSendSyncRequest(*amGetSessionHandle()))) return ret;

	*ticketReadCount = cmdbuf[2];

	return (Result)cmdbuf[1];
}

Result AMAPP_GetPatchTitleInfos(FS_MediaType mediatype, u32 titleCount, u64 *titleIds, AM_TitleEntry *titleInfo)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x100D,2,4); // 0x100D0084
	cmdbuf[1] = mediatype;
	cmdbuf[2] = titleCount;
	cmdbuf[3] = IPC_Desc_Buffer(titleCount*sizeof(u64),IPC_BUFFER_R);
	cmdbuf[4] = (u32)titleIds;
	cmdbuf[5] = IPC_Desc_Buffer(titleCount*sizeof(AM_TitleEntry),IPC_BUFFER_W);
	cmdbuf[6] = (u32)titleInfo;

	if(R_FAILED(ret = svcSendSyncRequest(*amGetSessionHandle()))) return ret;

	return (Result)cmdbuf[1];
}