#pragma once

#include <cstdint>
#include <string_view>

namespace tinyui {
	using WidgetIdValue = std::uint32_t;

	struct WidgetId {
		WidgetIdValue value = 0;
		
		constexpr bool IsValid() const {
			return value != 0;
		}

		static constexpr WidgetId Invalid() {
			return { 0 };
		}
	};
	
	inline constexpr bool operator==(WidgetId left, WidgetId right) noexcept {
		return left.value == right.value;
	}

	inline constexpr bool operator!=(WidgetId left, WidgetId right) noexcept {
		return left.value != right.value;
	}

	inline WidgetId MakeWidgetId(std::wstring_view text) {
		std::uint32_t hash = 2166136261u;
		for (wchar_t character : text) {
			hash ^= static_cast<std::uint32_t>(character);
			hash *= 16777619u;
		}

		if (hash == 0)
			hash = 1;
		
		return { hash };
	}
}