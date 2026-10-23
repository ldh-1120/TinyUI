#pragma once

#include <cstdint>
#include <expected>
#include <string>

namespace tinyui {
    enum class ErrorCode : std::uint8_t {
        Unknown,
        InvalidArgument,
        Unsupported,
        NotFound,
        PlatformFailure,
        GraphicsFailure,
        OutOfMemory,
        InternalFailure
    };

    struct Error {
        ErrorCode code = ErrorCode::Unknown;
        std::string message{};
        std::int64_t nativeCode = 0;
    };

    template <typename T>
    using Result = std::expected<T, Error>;
}
