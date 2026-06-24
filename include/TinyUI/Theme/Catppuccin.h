#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyUI/Theme/Theme.h>
#include <TinyUI/Theme/ThemeRegistry.h>

using namespace tinycore;

namespace tinyui {
    enum class CatppuccinFlavor {
        Latte,
        Frappe,
        Macchiato,
        Mocha
    };

    enum class CatppuccinColorName {
        Rosewater,
        Flamingo,
        Pink,
        Mauve,
        Red,
        Maroon,
        Peach,
        Yellow,
        Green,
        Teal,
        Sky,
        Sapphire,
        Blue,
        Lavender,
        Text,
        Subtext1,
        Subtext0,
        Overlay2,
        Overlay1,
        Overlay0,
        Surface2,
        Surface1,
        Surface0,
        Base,
        Mantle,
        Crust
    };

    struct CatppuccinPalette {
        Color rosewater { };
        Color flamingo { };
        Color pink { };
        Color mauve { };
        Color red { };
        Color maroon { };
        Color peach { };
        Color yellow { };
        Color green { };
        Color teal { };
        Color sky { };
        Color sapphire { };
        Color blue { };
        Color lavender { };
        Color text { };
        Color subtext1 { };
        Color subtext0 { };
        Color overlay2 { };
        Color overlay1 { };
        Color overlay0 { };
        Color surface2 { };
        Color surface1 { };
        Color surface0 { };
        Color base { };
        Color mantle { };
        Color crust { };

        Color Get(CatppuccinColorName name) const {
            switch (name) {
                case CatppuccinColorName::Rosewater:
                    return rosewater;

                case CatppuccinColorName::Flamingo:
                    return flamingo;

                case CatppuccinColorName::Pink:
                    return pink;

                case CatppuccinColorName::Mauve:
                    return mauve;

                case CatppuccinColorName::Red:
                    return red;

                case CatppuccinColorName::Maroon:
                    return maroon;

                case CatppuccinColorName::Peach:
                    return peach;

                case CatppuccinColorName::Yellow:
                    return yellow;

                case CatppuccinColorName::Green:
                    return green;

                case CatppuccinColorName::Teal:
                    return teal;

                case CatppuccinColorName::Sky:
                    return sky;

                case CatppuccinColorName::Sapphire:
                    return sapphire;

                case CatppuccinColorName::Blue:
                    return blue;

                case CatppuccinColorName::Lavender:
                    return lavender;

                case CatppuccinColorName::Text:
                    return text;

                case CatppuccinColorName::Subtext1:
                    return subtext1;

                case CatppuccinColorName::Subtext0:
                    return subtext0;

                case CatppuccinColorName::Overlay2:
                    return overlay2;

                case CatppuccinColorName::Overlay1:
                    return overlay1;

                case CatppuccinColorName::Overlay0:
                    return overlay0;

                case CatppuccinColorName::Surface2:
                    return surface2;

                case CatppuccinColorName::Surface1:
                    return surface1;

                case CatppuccinColorName::Surface0:
                    return surface0;

                case CatppuccinColorName::Base:
                    return base;

                case CatppuccinColorName::Mantle:
                    return mantle;

                case CatppuccinColorName::Crust:
                    return crust;
            }

            return base;
        }
    };
}

namespace tinyui::Catppuccin {
    namespace Latte {
        inline constexpr Color Rosewater = Color::FromHex(0xdc8a78);
        inline constexpr Color Flamingo = Color::FromHex(0xdd7878);
        inline constexpr Color Pink = Color::FromHex(0xea76cb);
        inline constexpr Color Mauve = Color::FromHex(0x8839ef);
        inline constexpr Color Red = Color::FromHex(0xd20f39);
        inline constexpr Color Maroon = Color::FromHex(0xe64553);
        inline constexpr Color Peach = Color::FromHex(0xfe640b);
        inline constexpr Color Yellow = Color::FromHex(0xdf8e1d);
        inline constexpr Color Green = Color::FromHex(0x40a02b);
        inline constexpr Color Teal = Color::FromHex(0x179299);
        inline constexpr Color Sky = Color::FromHex(0x04a5e5);
        inline constexpr Color Sapphire = Color::FromHex(0x209fb5);
        inline constexpr Color Blue = Color::FromHex(0x1e66f5);
        inline constexpr Color Lavender = Color::FromHex(0x7287fd);
        inline constexpr Color Text = Color::FromHex(0x4c4f69);
        inline constexpr Color Subtext1 = Color::FromHex(0x5c5f77);
        inline constexpr Color Subtext0 = Color::FromHex(0x6c6f85);
        inline constexpr Color Overlay2 = Color::FromHex(0x7c7f93);
        inline constexpr Color Overlay1 = Color::FromHex(0x8c8fa1);
        inline constexpr Color Overlay0 = Color::FromHex(0x9ca0b0);
        inline constexpr Color Surface2 = Color::FromHex(0xacb0be);
        inline constexpr Color Surface1 = Color::FromHex(0xbcc0cc);
        inline constexpr Color Surface0 = Color::FromHex(0xccd0da);
        inline constexpr Color Base = Color::FromHex(0xeff1f5);
        inline constexpr Color Mantle = Color::FromHex(0xe6e9ef);
        inline constexpr Color Crust = Color::FromHex(0xdce0e8);
    }

    namespace Frappe {
        inline constexpr Color Rosewater = Color::FromHex(0xf2d5cf);
        inline constexpr Color Flamingo = Color::FromHex(0xeebebe);
        inline constexpr Color Pink = Color::FromHex(0xf4b8e4);
        inline constexpr Color Mauve = Color::FromHex(0xca9ee6);
        inline constexpr Color Red = Color::FromHex(0xe78284);
        inline constexpr Color Maroon = Color::FromHex(0xea999c);
        inline constexpr Color Peach = Color::FromHex(0xef9f76);
        inline constexpr Color Yellow = Color::FromHex(0xe5c890);
        inline constexpr Color Green = Color::FromHex(0xa6d189);
        inline constexpr Color Teal = Color::FromHex(0x81c8be);
        inline constexpr Color Sky = Color::FromHex(0x99d1db);
        inline constexpr Color Sapphire = Color::FromHex(0x85c1dc);
        inline constexpr Color Blue = Color::FromHex(0x8caaee);
        inline constexpr Color Lavender = Color::FromHex(0xbabbf1);
        inline constexpr Color Text = Color::FromHex(0xc6d0f5);
        inline constexpr Color Subtext1 = Color::FromHex(0xb5bfe2);
        inline constexpr Color Subtext0 = Color::FromHex(0xa5adce);
        inline constexpr Color Overlay2 = Color::FromHex(0x949cbb);
        inline constexpr Color Overlay1 = Color::FromHex(0x838ba7);
        inline constexpr Color Overlay0 = Color::FromHex(0x737994);
        inline constexpr Color Surface2 = Color::FromHex(0x626880);
        inline constexpr Color Surface1 = Color::FromHex(0x51576d);
        inline constexpr Color Surface0 = Color::FromHex(0x414559);
        inline constexpr Color Base = Color::FromHex(0x303446);
        inline constexpr Color Mantle = Color::FromHex(0x292c3c);
        inline constexpr Color Crust = Color::FromHex(0x232634);
    }

    namespace Macchiato {
        inline constexpr Color Rosewater = Color::FromHex(0xf4dbd6);
        inline constexpr Color Flamingo = Color::FromHex(0xf0c6c6);
        inline constexpr Color Pink = Color::FromHex(0xf5bde6);
        inline constexpr Color Mauve = Color::FromHex(0xc6a0f6);
        inline constexpr Color Red = Color::FromHex(0xed8796);
        inline constexpr Color Maroon = Color::FromHex(0xee99a0);
        inline constexpr Color Peach = Color::FromHex(0xf5a97f);
        inline constexpr Color Yellow = Color::FromHex(0xeed49f);
        inline constexpr Color Green = Color::FromHex(0xa6da95);
        inline constexpr Color Teal = Color::FromHex(0x8bd5ca);
        inline constexpr Color Sky = Color::FromHex(0x91d7e3);
        inline constexpr Color Sapphire = Color::FromHex(0x7dc4e4);
        inline constexpr Color Blue = Color::FromHex(0x8aadf4);
        inline constexpr Color Lavender = Color::FromHex(0xb7bdf8);
        inline constexpr Color Text = Color::FromHex(0xcad3f5);
        inline constexpr Color Subtext1 = Color::FromHex(0xb8c0e0);
        inline constexpr Color Subtext0 = Color::FromHex(0xa5adcb);
        inline constexpr Color Overlay2 = Color::FromHex(0x939ab7);
        inline constexpr Color Overlay1 = Color::FromHex(0x8087a2);
        inline constexpr Color Overlay0 = Color::FromHex(0x6e738d);
        inline constexpr Color Surface2 = Color::FromHex(0x5b6078);
        inline constexpr Color Surface1 = Color::FromHex(0x494d64);
        inline constexpr Color Surface0 = Color::FromHex(0x363a4f);
        inline constexpr Color Base = Color::FromHex(0x24273a);
        inline constexpr Color Mantle = Color::FromHex(0x1e2030);
        inline constexpr Color Crust = Color::FromHex(0x181926);
    }

    namespace Mocha {
        inline constexpr Color Rosewater = Color::FromHex(0xf5e0dc);
        inline constexpr Color Flamingo = Color::FromHex(0xf2cdcd);
        inline constexpr Color Pink = Color::FromHex(0xf5c2e7);
        inline constexpr Color Mauve = Color::FromHex(0xcba6f7);
        inline constexpr Color Red = Color::FromHex(0xf38ba8);
        inline constexpr Color Maroon = Color::FromHex(0xeba0ac);
        inline constexpr Color Peach = Color::FromHex(0xfab387);
        inline constexpr Color Yellow = Color::FromHex(0xf9e2af);
        inline constexpr Color Green = Color::FromHex(0xa6e3a1);
        inline constexpr Color Teal = Color::FromHex(0x94e2d5);
        inline constexpr Color Sky = Color::FromHex(0x89dceb);
        inline constexpr Color Sapphire = Color::FromHex(0x74c7ec);
        inline constexpr Color Blue = Color::FromHex(0x89b4fa);
        inline constexpr Color Lavender = Color::FromHex(0xb4befe);
        inline constexpr Color Text = Color::FromHex(0xcdd6f4);
        inline constexpr Color Subtext1 = Color::FromHex(0xbac2de);
        inline constexpr Color Subtext0 = Color::FromHex(0xa6adc8);
        inline constexpr Color Overlay2 = Color::FromHex(0x9399b2);
        inline constexpr Color Overlay1 = Color::FromHex(0x7f849c);
        inline constexpr Color Overlay0 = Color::FromHex(0x6c7086);
        inline constexpr Color Surface2 = Color::FromHex(0x585b70);
        inline constexpr Color Surface1 = Color::FromHex(0x45475a);
        inline constexpr Color Surface0 = Color::FromHex(0x313244);
        inline constexpr Color Base = Color::FromHex(0x1e1e2e);
        inline constexpr Color Mantle = Color::FromHex(0x181825);
        inline constexpr Color Crust = Color::FromHex(0x11111b);
    }
}

namespace tinyui::Catppuccin {
    inline constexpr CatppuccinPalette LattePalette {
        Latte::Rosewater,
        Latte::Flamingo,
        Latte::Pink,
        Latte::Mauve,
        Latte::Red,
        Latte::Maroon,
        Latte::Peach,
        Latte::Yellow,
        Latte::Green,
        Latte::Teal,
        Latte::Sky,
        Latte::Sapphire,
        Latte::Blue,
        Latte::Lavender,
        Latte::Text,
        Latte::Subtext1,
        Latte::Subtext0,
        Latte::Overlay2,
        Latte::Overlay1,
        Latte::Overlay0,
        Latte::Surface2,
        Latte::Surface1,
        Latte::Surface0,
        Latte::Base,
        Latte::Mantle,
        Latte::Crust
    };

    inline constexpr CatppuccinPalette FrappePalette {
        Frappe::Rosewater,
        Frappe::Flamingo,
        Frappe::Pink,
        Frappe::Mauve,
        Frappe::Red,
        Frappe::Maroon,
        Frappe::Peach,
        Frappe::Yellow,
        Frappe::Green,
        Frappe::Teal,
        Frappe::Sky,
        Frappe::Sapphire,
        Frappe::Blue,
        Frappe::Lavender,
        Frappe::Text,
        Frappe::Subtext1,
        Frappe::Subtext0,
        Frappe::Overlay2,
        Frappe::Overlay1,
        Frappe::Overlay0,
        Frappe::Surface2,
        Frappe::Surface1,
        Frappe::Surface0,
        Frappe::Base,
        Frappe::Mantle,
        Frappe::Crust
    };

    inline constexpr CatppuccinPalette MacchiatoPalette {
        Macchiato::Rosewater,
        Macchiato::Flamingo,
        Macchiato::Pink,
        Macchiato::Mauve,
        Macchiato::Red,
        Macchiato::Maroon,
        Macchiato::Peach,
        Macchiato::Yellow,
        Macchiato::Green,
        Macchiato::Teal,
        Macchiato::Sky,
        Macchiato::Sapphire,
        Macchiato::Blue,
        Macchiato::Lavender,
        Macchiato::Text,
        Macchiato::Subtext1,
        Macchiato::Subtext0,
        Macchiato::Overlay2,
        Macchiato::Overlay1,
        Macchiato::Overlay0,
        Macchiato::Surface2,
        Macchiato::Surface1,
        Macchiato::Surface0,
        Macchiato::Base,
        Macchiato::Mantle,
        Macchiato::Crust
    };

    inline constexpr CatppuccinPalette MochaPalette {
        Mocha::Rosewater,
        Mocha::Flamingo,
        Mocha::Pink,
        Mocha::Mauve,
        Mocha::Red,
        Mocha::Maroon,
        Mocha::Peach,
        Mocha::Yellow,
        Mocha::Green,
        Mocha::Teal,
        Mocha::Sky,
        Mocha::Sapphire,
        Mocha::Blue,
        Mocha::Lavender,
        Mocha::Text,
        Mocha::Subtext1,
        Mocha::Subtext0,
        Mocha::Overlay2,
        Mocha::Overlay1,
        Mocha::Overlay0,
        Mocha::Surface2,
        Mocha::Surface1,
        Mocha::Surface0,
        Mocha::Base,
        Mocha::Mantle,
        Mocha::Crust
    };

    inline const CatppuccinPalette& GetPalette(CatppuccinFlavor flavor) {
        switch (flavor) {
            case CatppuccinFlavor::Latte:
                return LattePalette;

            case CatppuccinFlavor::Frappe:
                return FrappePalette;

            case CatppuccinFlavor::Macchiato:
                return MacchiatoPalette;

            case CatppuccinFlavor::Mocha:
                return MochaPalette;
        }

        return MochaPalette;
    }
}

namespace tinyui {
    inline Theme MakeCatppuccinTheme(const CatppuccinPalette& palette, const wchar_t* name) {
        Theme theme {};
        theme.name = name;

        theme.accent = palette.mauve;
        theme.accentHovered = palette.lavender;

        theme.success = palette.green;
        theme.warning = palette.yellow;
        theme.error = palette.red;

        theme.text.primary = palette.text;
        theme.text.secondary = palette.subtext1;
        theme.text.muted = palette.subtext0;
        theme.text.disabled = palette.overlay0;

        theme.window.background = palette.base;
        theme.window.backgroundAlt = palette.mantle;
        theme.window.border = palette.surface0;

        theme.panel.background = palette.mantle;
        theme.panel.backgroundHovered = palette.surface0;
        theme.panel.header = palette.surface0;
        theme.panel.border = palette.surface1;
        theme.panel.titleText = palette.text;

        theme.button.normal = palette.surface0;
        theme.button.hovered = palette.surface1;
        theme.button.pressed = palette.surface2;
        theme.button.border = palette.surface2;
        theme.button.text = palette.text;

        theme.slider.track = palette.surface0;
        theme.slider.trackHovered = palette.surface1;
        theme.slider.fill = palette.mauve;
        theme.slider.handle = palette.text;
        theme.slider.border = palette.surface2;
        theme.slider.labelText = palette.text;
        theme.slider.valueText = palette.subtext1;

        theme.textBox.background = palette.surface0;
        theme.textBox.backgroundHovered = palette.surface1;
        theme.textBox.backgroundFocused = palette.surface0;
        theme.textBox.border = palette.surface2;
        theme.textBox.borderFocused = palette.mauve;
        theme.textBox.text = palette.text;
        theme.textBox.placeholder = palette.overlay1;
        theme.textBox.cursor = palette.text;
        theme.textBox.selection = palette.mauve;
        theme.textBox.selection.a = 0.35f;

        theme.checkbox.boxBackground = palette.surface0;
        theme.checkbox.boxHovered = palette.surface1;
        theme.checkbox.boxChecked = palette.mauve;
        theme.checkbox.border = palette.surface2;
        theme.checkbox.checkmark = palette.base;
        theme.checkbox.text = palette.text;

        theme.toggle.trackOff = palette.surface0;
        theme.toggle.trackOffHovered = palette.surface1;
        theme.toggle.trackOn = palette.mauve;
        theme.toggle.trackOnHovered = palette.lavender;
        theme.toggle.knob = palette.text;
        theme.toggle.border = palette.surface2;
        theme.toggle.text = palette.text;

        theme.scrollPanel.background = palette.mantle;
        theme.scrollPanel.border = palette.surface1;
        theme.scrollPanel.scrollbarTrack = palette.surface0;
        theme.scrollPanel.scrollbarThumb = palette.overlay0;
        theme.scrollPanel.scrollbarThumbHovered = palette.overlay1;

        theme.dropdown.background = palette.surface0;
        theme.dropdown.backgroundHovered = palette.surface1;
        theme.dropdown.backgroundOpened = palette.surface1;
        theme.dropdown.border = palette.surface2;
        theme.dropdown.borderFocused = palette.mauve;
        theme.dropdown.text = palette.text;
        theme.dropdown.placeholder = palette.overlay1;
        theme.dropdown.arrow = palette.text;
        theme.dropdown.menuBackground = palette.mantle;
        theme.dropdown.itemHovered = palette.surface0;
        theme.dropdown.itemSelected = palette.mauve;
        theme.dropdown.itemSelected.a = 0.35f;

        theme.tabBar.background = palette.mantle;
        theme.tabBar.tabNormal = palette.surface0;
        theme.tabBar.tabHovered = palette.surface1;
        theme.tabBar.tabPressed = palette.surface2;
        theme.tabBar.tabSelected = palette.surface1;
        theme.tabBar.border = palette.surface2;
        theme.tabBar.accent = palette.mauve;
        theme.tabBar.text = palette.subtext1;
        theme.tabBar.selectedText = palette.text;

        theme.tooltip.background = palette.surface0;
        theme.tooltip.border = palette.surface2;
        theme.tooltip.text = palette.text;

        theme.label.text = palette.text;
        theme.label.mutedText = palette.subtext1;
        theme.label.headingText = palette.text;

        theme.separator.line = palette.surface2;
        theme.separator.text = palette.subtext1;

        theme.progressBar.background = palette.surface0;
        theme.progressBar.fill = palette.mauve;
        theme.progressBar.border = palette.surface2;
        theme.progressBar.text = palette.text;

        theme.modal.overlay = palette.crust;
        theme.modal.overlay.a = 0.62f;
        theme.modal.background = palette.mantle;
        theme.modal.border = palette.surface2;
        theme.modal.titleText = palette.text;
        theme.modal.messageText = palette.subtext1;

        return theme;
    }

    inline Theme MakeCatppuccinLatteTheme() {
        return MakeCatppuccinTheme(Catppuccin::LattePalette, L"Catppuccin Latte");
    }

    inline Theme MakeCatppuccinFrappeTheme() {
        return MakeCatppuccinTheme(Catppuccin::FrappePalette, L"Catppuccin Frappe");
    }

    inline Theme MakeCatppuccinMacchiatoTheme() {
        return MakeCatppuccinTheme(Catppuccin::MacchiatoPalette, L"Catppuccin Macchiato");
    }

    inline Theme MakeCatppuccinMochaTheme() {
        return MakeCatppuccinTheme(Catppuccin::MochaPalette, L"Catppuccin Mocha");
    }

    inline Theme MakeDefaultTheme() {
        return MakeCatppuccinMochaTheme();
    }
}

namespace tinyui {
    inline ThemeRegistry MakeCatppuccinThemeRegistry() {
        ThemeRegistry registry {};

        registry.RegisterTheme(L"catppuccin.mocha", MakeCatppuccinMochaTheme());
        registry.RegisterTheme(L"catppuccin.macchiato", MakeCatppuccinMacchiatoTheme());
        registry.RegisterTheme(L"catppuccin.frappe", MakeCatppuccinFrappeTheme());
        registry.RegisterTheme(L"catppuccin.latte", MakeCatppuccinLatteTheme());
        registry.SetCurrentTheme(L"catppuccin.mocha");

        return registry;
    }
}