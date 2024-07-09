#pragma once
#include "3ds.h"

typedef struct
{
    u64 title_id;
    u64 ticket_id;
    u16 version;
    u16 unused;
    u32 size;
} AM_TicketInfo;

Result AM_GetTitleInfoIgnorePlatform(FS_MediaType mediatype, u32 titleCount, u64 *titleIds, AM_TitleEntry *titleInfo);

Result AMAPP_FindDLCContentInfos(FS_MediaType mediatype, u64 title_id, u32 contentCount, u16 *contentIds, AM_ContentInfo *contentInfos);

Result AMAPP_GetDLCTitleInfos(FS_MediaType mediatype, u32 titleCount, u64 *titleIds, AM_TitleEntry *titleInfo);

Result AMAPP_ListDataTitleTicketInfos(u32* ticketReadCount, u64 titleID, u32 ticketInfoCount, u32 offset, AM_TicketInfo* ticketInfos);

Result AMAPP_GetPatchTitleInfos(FS_MediaType mediatype, u32 titleCount, u64 *titleIds, AM_TitleEntry *titleInfo);