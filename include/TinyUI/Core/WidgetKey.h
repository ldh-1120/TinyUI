#pragma once

#include <cstdint>
#include <string_view>

namespace tinyui {
	using WidgetKeyValue = std::uint64_t;

	struct WidgetKey {
		WidgetKeyValue value = 0;

		bool IsValid() const {
			return value != 0;
		}

		static constexpr WidgetKey Invalid() {
			return { 0 };
		}
	};

	inline constexpr bool operator==(WidgetKey left, WidgetKey right) {
		return left.value == right.value;
	}

	inline constexpr bool operator!=(WidgetKey left, WidgetKey right) {
		return left.value != right.value;
	}

	inline WidgetKey MakeWidgetKey(std::wstring_view text) {
		std::uint64_t hash = 14695981039346656037ull;
		for (wchar_t character : text) {
			hash ^= static_cast<std::uint64_t>(character);
			hash *= 1099511628211ull;
		}

		if (hash == 0)
			hash = 1;

		return { hash };
	}
}