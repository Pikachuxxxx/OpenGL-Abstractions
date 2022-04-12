#pragma once

#include <cstdint>

namespace GISurfels {

    struct SurfelCluster
    {
        uint32_t surfelOffset = 0;
        uint32_t surfelCount = 0;
        glm::vec3 center;

        SurfelCluster() = default;
        SurfelCluster(size_t offset, size_t count)
            : surfelOffset(offset), surfelCount(count) {}
    };
}