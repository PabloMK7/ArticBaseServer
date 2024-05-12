#pragma once
#include "3ds.h"

Result FSUSER_NewSetSaveDataSecureValue(FS_Archive archive, u64 value, FS_SecureValueSlot slot, bool flush);

Result FSUSER_NewGetSaveDataSecureValue(bool* exists, bool* isGamecard, u64* value, FS_Archive archive, FS_SecureValueSlot slot);

Result FSUSER_SetThisSaveDataSecureValue(u64 value, FS_SecureValueSlot slot);

Result FSUSER_GetThisSaveDataSecureValue(bool* exists, bool* isGamecard, u64* value, FS_SecureValueSlot slot);