#pragma once

#include <cstdint>

#include "Math/SphericalHarmonics.h"

namespace GISurfels {

    struct SurfelClusterProjection
    {
        uint32_t surfelClusterIndex = 0;
        SphericalHarmonics sphericalHarmonics;
    };
}