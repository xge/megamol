/*
 * GUIUtils.h
 *
 * Copyright (C) 2019 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#pragma once

#include <string>

#include <imgui.h>


namespace megamol {
namespace gui {

/**
 * Utility class for GUIUtils-style widgets.
 */
class GUIUtils {
public:
    GUIUtils(void);

    ~GUIUtils(void) = default;

    /**
     * Show tooltip on hover.
     *
     * @param text        The tooltip text.
     * @param id          The id of the imgui item the tooltip belongs (only needed for delayed appearance of tooltip).
     * @param time_start  The time delay to wait until the tooltip is shown for a hovered imgui item.
     * @param time_end    The time delay to wait until the tooltip is hidden for a hovered imgui item.
     */
    void HoverToolTip(std::string text, ImGuiID id = 0, float time_start = 0.0f, float time_end = 4.0f);

    /**
     * Show help marker text with tooltip on hover.
     *
     * @param text   The help tooltip text.
     * @param label  The visible text for which the tooltip is enabled.
     */
    void HelpMarkerToolTip(std::string text, std::string label = "(?)");

    /**
     * Returns width of text drawn as widget.
     */
    float TextWidgetWidth(std::string text);

    /** Decode string from UTF-8. */
    std::string utf8Decode(std::string str);

    /** Encode string into UTF-8. */
    std::string utf8Encode(std::string str);

private:
    /** Current tooltip hover time. */
    float tooltipTime;

    /** Current hovered tooltip item. */
    ImGuiID tooltipId;
};


} // namespace gui
} // namespace megamol