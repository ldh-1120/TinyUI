#pragma once

#include <cstdint>
#include <limits>

namespace tinyui {
    template<typename Tag>
    struct Handle {
        static constexpr std::uint32_t InvalidIndex = std::numeric_limits<std::uint32_t>::max();

        std::uint32_t index = InvalidIndex;
        std::uint32_t generation = 0;
        std::uint64_t owner = 0;

        [[nodiscard]] constexpr bool valid() const noexcept {
            return index != InvalidIndex && generation != 0 && owner != 0;
        }

        [[nodiscard]] constexpr bool operator==(const Handle &other) const noexcept = default;
    };
}
