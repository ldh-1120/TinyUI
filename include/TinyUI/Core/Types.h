#pragma once

namespace tinyui {
	struct Vec2 {
		float x = 0.f;
		float y = 0.f;
	};

	struct Size {
		float width = 0.f;
		float height = 0.f;
	};

	struct Rect {
		float x = 0.f;
		float y = 0.f;
		float w = 0.f;
		float h = 0.f;

		float Left() const {
			return x;
		}

		float Top() const {
			return y;
		}

		float Right() const {
			return x + w;
		}

		float Bottom() const {
			return y + h;
		}

		Vec2 Position() const {
			return { x, y };
		}

		Size GetSize() const {
			return { w, h };
		}

		bool Contains(Vec2 point) const {
			return point.x >= x && point.x <= x + w && point.y >= y && point.y <= y + h;
		}
	};

	struct Thickness {
		float left = 0.f;
		float top = 0.f;
		float right = 0.f;
		float bottom = 0.f;

		static Thickness All(float value) {
			return { value, value, value, value };
		}

		static Thickness HorizontalVertical(float horizontal, float vertical) {
			return { horizontal, vertical, horizontal, vertical };
		}
	};

	struct Color {
		float r = 1.f;
		float g = 1.f;
		float b = 1.f;
		float a = 1.f;

		static constexpr Color FromRGB(int r, int g, int b, int a = 255) {
			return { r / 255.f, g / 255.f, b / 255.f, a / 255.f };
		}

		static constexpr Color FromHex(unsigned int rgb, int alpha = 255) {
			return FromRGB(static_cast<int>((rgb >> 16) & 0xff), static_cast<int>((rgb >> 8) & 0xff), static_cast<int>(rgb & 0xff), alpha);
		}
	};
}