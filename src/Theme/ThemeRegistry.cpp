#include "pch.h"

#include <TinyUI/Theme/ThemeRegistry.h>

namespace tinyui {
	static bool EqualsStringView(const std::wstring& text, std::wstring_view view) {
		std::wstring_view textView(text.data(), text.size());
		return textView == view;
	}

	void ThemeRegistry::Clear() {
		m_entries.clear();
		m_currentThemeId.clear();
	}

	void ThemeRegistry::RegisterTheme(std::wstring_view id, const Theme& theme) {
		ThemeEntry* existingEntry = FindEntry(id);
		if (existingEntry) {
			existingEntry->theme = theme;
			return;
		}

		ThemeEntry entry { };
		entry.id = std::wstring(id);
		entry.theme = theme;

		m_entries.push_back(entry);
		if (m_currentThemeId.empty())
			m_currentThemeId = entry.id;
	}

	bool ThemeRegistry::RemoveTheme(std::wstring_view id) {
		for (std::size_t index = 0; index < m_entries.size(); ++index) {
			if (!EqualsStringView(m_entries[index].id, id))
				continue;

			const bool removingCurrent = EqualsStringView(m_currentThemeId, id);
			m_entries.erase(m_entries.begin() + static_cast<std::ptrdiff_t>(index));
			if (removingCurrent) {
				m_currentThemeId.clear();
				if (!m_entries.empty())
					m_currentThemeId = m_entries[0].id;
			}

			return true;
		}

		return false;
	}

	bool ThemeRegistry::HasTheme(std::wstring_view id) {
		return FindEntry(id) != nullptr;
	}
 
	Theme* ThemeRegistry::FindTheme(std::wstring_view id) {
		ThemeEntry* entry = FindEntry(id);
		if (!entry)
			return nullptr;

		return &entry->theme;
	}

	const Theme* ThemeRegistry::FindTheme(std::wstring_view id) const {
		const ThemeEntry* entry = FindEntry(id);
		if (!entry)
			return nullptr;

		return &entry->theme;
	}

	bool ThemeRegistry::SetCurrentTheme(std::wstring_view id) {
		if (!FindEntry(id))
			return false;

		m_currentThemeId = std::wstring(id);
		return true;
	}

	Theme& ThemeRegistry::GetCurrentTheme() {
		ThemeEntry* entry = FindEntry(m_currentThemeId);
		if (!entry)
			return m_fallbackTheme;

		return entry->theme;
	}

	const Theme& ThemeRegistry::GetCurrentTheme() const {
		const ThemeEntry* entry = FindEntry(m_currentThemeId);
		if (!entry)
			return m_fallbackTheme;

		return entry->theme;
	}

	std::wstring_view ThemeRegistry::GetCurrentThemeId() const {
		return std::wstring_view(m_currentThemeId.data(), m_currentThemeId.size());
	}

	std::size_t ThemeRegistry::GetThemeCount() const {
		return m_entries.size();
	}

	const ThemeEntry& ThemeRegistry::GetThemeEntry(std::size_t index) const {
		return m_entries[index];
	}

	ThemeEntry* ThemeRegistry::FindEntry(std::wstring_view id) {
		for (ThemeEntry& entry : m_entries) {
			if (EqualsStringView(entry.id, id))
				return &entry;
		}

		return nullptr;
	}

	const ThemeEntry* ThemeRegistry::FindEntry(std::wstring_view id) const {
		for (const ThemeEntry& entry : m_entries) {
			if (EqualsStringView(entry.id, id))
				return &entry;
		}

		return nullptr;
	}
}