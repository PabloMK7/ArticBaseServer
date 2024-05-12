#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <3ds/types.h>

typedef struct
{
    bool    noFlash;
    u8      pluginMemoryStrategy;
    u8      persistent;
    u32     lowTitleId;
    char    path[256];
    u32     config[32];
}   PluginLoadParameters;

typedef enum
{
    PLG_STRATEGY_NONE = 2,
    PLG_STRATEGY_SWAP = 0,
    PLG_STRATEGY_MODE3 = 1
} PluginMemoryStrategy;

Result  plgLdrInit(void);
void    plgLdrExit(void);
Result  PLGLDR__IsPluginLoaderEnabled(bool *isEnabled);
Result  PLGLDR__SetPluginLoaderState(bool enabled);
Result  PLGLDR__SetPluginLoadParameters(PluginLoadParameters *parameters);
Result  PLGLDR__GetVersion(u32* version);
Result  PLGLDR__SetExeDecSettings(void* decFunc, void* args);
Result  PLGLDR__ClearPluginLoadParameters();

#ifdef __cplusplus
}
#endif
