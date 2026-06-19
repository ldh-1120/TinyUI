#pragma once

#include <TinyUI/Theme/Theme.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace tinyui {
	struct ThemeEntry {
		std::wstring id { };
		Theme theme { };
	};

	class ThemeRegistry {
	public:
		void Clear();

		void RegisterTheme(std::wstring_view id, const Theme& theme);
		bool RemoveTheme(std::wstring_view id);

		bool HasTheme(std::wstring_view id);

		Theme* FindTheme(std::wstring_view id);
		const Theme* FindTheme(std::wstring_view id) const;

		bool SetCurrentTheme(std::wstring_view id);

		Theme& GetCurrentTheme();
		const Theme& GetCurrentTheme() const;

		std::wstring_view GetCurrentThemeId() const;

		std::size_t GetThemeCount() const;
		const ThemeEntry& GetThemeEntry(std::size_t index) const;

	private:
		ThemeEntry* FindEntry(std::wstring_view id);
		const ThemeEntry* FindEntry(std::wstring_view id) const;

	private:
		std::vector<ThemeEntry> m_entries { };
		std::wstring m_currentThemeId { };

		Theme m_fallbackTheme { };
	};
}