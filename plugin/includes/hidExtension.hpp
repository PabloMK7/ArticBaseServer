#pragma once
#include "3ds.h"

struct GyroscopeCalibrateParam {
    struct {
        s16 zero_point;
        s16 positive_unit_point;
        s16 negative_unit_point;
    } x, y, z;
};

Result HIDUSER_GetGyroscopeCalibrateParam(GyroscopeCalibrateParam* calibrateParam);