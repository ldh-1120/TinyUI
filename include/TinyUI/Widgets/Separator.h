#pragma once

#include <TinyCore/Core/Types.h>

#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
    enum class SeparatorDirection { Horizontal, Vertical };

    struct SeparatorOptions {
        float thickness = 1.0f;

        tinycore::Color color { };
        bool useCustomColor = false;
    };

    class Separator final : public Widget {
    public:
        Separator();

        void SetDirection(SeparatorDirection direction);
        SeparatorDirection GetDirection() const;

        void SetOptions(const SeparatorOptions& options);
        const SeparatorOptions& GetOptions() const;

    protected:
        void OnPaint(PaintContext& context) override;

    private:
        float GetThickness() const;
        tinycore::Color GetColor(const Theme& theme) const;

    private:
        SeparatorDirection m_direction = SeparatorDirection::Horizontal;
        SeparatorOptions m_options { };
    };
}