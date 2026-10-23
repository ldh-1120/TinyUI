#pragma once

namespace tinyui {
    struct Point {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Size {
        float width = 0.0f;
        float height = 0.0f;

        [[nodiscard]] constexpr bool empty() const noexcept {
            return width <= 0.0f || height <= 0.0f;
        }
    };

    struct Rect {
        Point origin{};
        Size size{};

        [[nodiscard]] constexpr float left() const noexcept {
            return origin.x;
        }

        [[nodiscard]] constexpr float top() const noexcept {
            return origin.y;
        }

        [[nodiscard]] constexpr float right() const noexcept {
            return origin.x + size.width;
        }

        [[nodiscard]] constexpr float bottom() const noexcept {
            return origin.y + size.height;
        }

        [[nodiscard]] constexpr bool contains(Point point) const noexcept {
            return !size.empty() && point.x >= origin.x && point.x < right() && point.y >= origin.y && point.y < bottom();
        }
    };
}