#include "3ds/types.h"

#define _3GX_MAGIC (0x3230303024584733) /* "3GX$0002" */

typedef struct CTR_PACKED
{
    u32             authorLen;
    const char*     authorMsg;
    u32             titleLen;
    const char*     titleMsg;
    u32             summaryLen;
    const char*     summaryMsg;
    u32             descriptionLen;
    const char*     descriptionMsg;
    union {
        u32         flags;
        struct {
            u32     embeddedExeLoadFunc : 1;
            u32     embeddedSwapSaveLoadFunc : 1;
            u32     memoryRegionSize : 2;
            u32     compatibility : 2;
            u32     eventsSelfManaged : 1;
            u32     swapNotNeeded : 1;
            u32     unused : 24;
        };
    };
    u32             exeLoadChecksum;
    u32             builtInLoadExeArgs[4];
    u32             builtInSwapSaveLoadArgs[4];
} _3gx_Infos;

typedef struct CTR_PACKED
{
    u32             count;
    u32           * titles;
}   _3gx_Targets;

typedef struct CTR_PACKED
{
    u32             nbSymbols;
    u32             symbolsOffset;
    u32             nameTableOffset;
}   _3gx_Symtable;

typedef struct CTR_PACKED
{
    u32             codeOffset;
    u32             rodataOffset;
    u32             dataOffset;
    u32             codeSize;
    u32             rodataSize;
    u32             dataSize;
    u32             bssSize;
    u32             exeLoadFuncOffset; // NOP terminated
    u32             swapSaveFuncOffset; // NOP terminated
    u32             swapLoadFuncOffset; // NOP terminated
} _3gx_Executable;

typedef struct CTR_PACKED
{
    u64             magic;
    u32             version;
    u32             reserved;
    _3gx_Infos      infos;
    _3gx_Executable executable;
    _3gx_Targets    targets;
    _3gx_Symtable   symtable;
} _3gx_Header;