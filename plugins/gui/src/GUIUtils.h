/*
 * GUIUtils.h
 *
 * Copyright (C) 2019 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOL_GUI_GUIUTILS_INCLUDED
#define MEGAMOL_GUI_GUIUTILS_INCLUDED


#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#include <imgui.h>
#include <imgui_internal.h>

#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "json.hpp"

#include <algorithm> // search
#include <array>
#include <cctype> // toupper
#include <cmath>  // fmodf
#include <list>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "mmcore/param/AbstractParamPresentation.h"
#include "mmcore/view/Input.h"

#include "vislib/UTF8Encoder.h"
#include "vislib/sys/Log.h"


namespace megamol {
namespace gui {


/********** Defines **********/

/// #define GUI_VERBOSE

#define GUI_INVALID_ID (UINT_MAX)
#define GUI_SLOT_RADIUS (8.0f)
#define GUI_LINE_THICKNESS (3.0f)
#define GUI_RECT_CORNER_RADIUS (0.0f)
#define GUI_MAX_MULITLINE (7)
#define GUI_DND_CALLSLOT_UID_TYPE ("DND_CALL")
#define GUI_GRAPH_BORDER (GUI_SLOT_RADIUS * 4.0f)
#define GUI_MULTISELECT_MODIFIER (ImGui::GetIO().KeyShift)

#define GUI_MODULE_NAME ("GUIView")
#define GUI_GUI_STATE_PARAM_NAME ("state")
#define GUI_CONFIGURATOR_STATE_PARAM_NAME ("configurator::state")

#define GUI_JSON_TAG_WINDOW_CONFIGURATIONS ("WindowConfigurations")
#define GUI_JSON_TAG_GUISTATE ("GUI")
#define GUI_JSON_TAG_CONFIGURATOR ("Configurator")
#define GUI_JSON_TAG_GRAPHS ("Graphs")

// Global Colors
#define GUI_COLOR_TEXT_ERROR (ImVec4(0.9f, 0.0f, 0.0f, 1.0f))
#define GUI_COLOR_TEXT_WARN (ImVec4(0.75f, 0.75f, 0.f, 1.0f))
#define GUI_COLOR_BUTTON_MODIFIED (ImVec4(0.6f, 0.0f, 0.3f, 1.0f))
#define GUI_COLOR_BUTTON_MODIFIED_HIGHLIGHT (ImVec4(0.9f, 0.0f, 0.45f, 1.0f))
#define GUI_COLOR_SLOT_CALLER (ImVec4(0.0f, 1.0f, 0.75f, 1.0f))
#define GUI_COLOR_SLOT_CALLEE (ImVec4(0.75f, 0.0f, 1.0f, 1.0f))
#define GUI_COLOR_SLOT_COMPATIBLE (ImVec4(0.75f, 1.0f, 0.25f, 1.0f))

/********** Types **********/

// Forward declaration
class CallSlot;
class InterfaceSlot;
typedef std::shared_ptr<megamol::gui::CallSlot> CallSlotPtrType;
typedef std::shared_ptr<megamol::gui::InterfaceSlot> InterfaceSlotPtrType;

/** Available ImGui APIs */
enum GUIImGuiAPI { NONE, OpenGL };

/** Hotkey Data Types (exclusively for configurator) */
enum HotkeyIndex : size_t {
    MODULE_SEARCH = 0,
    PARAMETER_SEARCH = 1,
    DELETE_GRAPH_ITEM = 2,
    SAVE_PROJECT = 3,
    INDEX_COUNT = 4
};
typedef std::tuple<megamol::core::view::KeyCode, bool> HotkeyDataType;
typedef std::array<megamol::gui::HotkeyDataType, megamol::gui::HotkeyIndex::INDEX_COUNT> HotkeyArrayType;

typedef megamol::core::param::AbstractParamPresentation::Presentation PresentType;
typedef megamol::core::param::AbstractParamPresentation::ParamType ParamType;
typedef std::map<int, std::string> EnumStorageType;

typedef std::array<float, 5> FontScalingArrayType;

/* Data type holding a pair of uids. */
typedef std::vector<ImGuiID> UIDVectorType;
typedef std::pair<ImGuiID, ImGuiID> UIDPairType;
typedef std::vector<UIDPairType> UIDPairVectorType;

/* Data type holding current group uid and group name pairs. */
typedef std::pair<ImGuiID, std::string> GroupPairType;
typedef std::vector<megamol::gui::GroupPairType> GroupPairVectorType;

enum PresentPhase : size_t { INTERACTION = 0, RENDERING = 1 };

/* Data type holding information of graph canvas. */
typedef struct _canvas_ {
    ImVec2 position;  // in
    ImVec2 size;      // in
    ImVec2 scrolling; // in
    float zooming;    // in
    ImVec2 offset;    // in
} GraphCanvasType;

/* Data type holding information on graph item interaction. */
typedef struct _interact_state_ {
    ImGuiID button_active_uid;  // in out
    ImGuiID button_hovered_uid; // in out
    bool process_deletion;      // out

    ImGuiID group_selected_uid; // in out
    ImGuiID group_hovered_uid;  // in out
    bool group_layout;          // out

    UIDVectorType modules_selected_uids;      // in out
    ImGuiID module_hovered_uid;               // in out
    ImGuiID module_mainview_uid;              // out
    UIDPairVectorType modules_add_group_uids; // out
    UIDVectorType modules_remove_group_uids;  // out
    bool modules_layout;                      // out

    ImGuiID call_selected_uid; // in out
    ImGuiID call_hovered_uid;  // in out

    ImGuiID slot_dropped_uid; // in out

    ImGuiID callslot_selected_uid;         // in out
    ImGuiID callslot_hovered_uid;          // in out
    UIDPairType callslot_add_group_uid;    // in out
    UIDPairType callslot_remove_group_uid; // in out
    CallSlotPtrType callslot_compat_ptr;   // in

    ImGuiID interfaceslot_selected_uid;            // in out
    ImGuiID interfaceslot_hovered_uid;             // in out
    InterfaceSlotPtrType interfaceslot_compat_ptr; // in

} GraphItemsInteractType;

/* Data type holding shared state of graph items. */
typedef struct _graph_item_state_ {
    megamol::gui::GraphCanvasType canvas;          // (see above)
    megamol::gui::GraphItemsInteractType interact; // (see above)
    megamol::gui::HotkeyArrayType hotkeys;         // in out
    megamol::gui::GroupPairVectorType groups;      // in
} GraphItemsStateType;

/* Data type holding shared state of graphs. */
typedef struct _graph_state_ {
    FontScalingArrayType font_scalings;    // in
    float graph_width;                     // in
    bool show_parameter_sidebar;           // in
    megamol::gui::HotkeyArrayType hotkeys; // in out
    ImGuiID graph_selected_uid;            // out
    bool graph_delete;                     // out
    bool graph_save;                       // out
} GraphStateType;


/********** Global Unique ID **********/

extern ImGuiID gui_generated_uid;
static ImGuiID GenerateUniqueID(void) { return (++megamol::gui::gui_generated_uid); }


/********** Class **********/

/**
 * Static GUI utility functions.
 */
class GUIUtils {
public:
    /** Decode string from UTF-8. */
    static bool Utf8Decode(std::string& str) {

        vislib::StringA dec_tmp;
        if (vislib::UTF8Encoder::Decode(dec_tmp, vislib::StringA(str.c_str()))) {
            str = std::string(dec_tmp.PeekBuffer());
            return true;
        }
        return false;
    }

    /** Encode string into UTF-8. */
    static bool Utf8Encode(std::string& str) {

        vislib::StringA dec_tmp;
        if (vislib::UTF8Encoder::Encode(dec_tmp, vislib::StringA(str.c_str()))) {
            str = std::string(dec_tmp.PeekBuffer());
            return true;
        }
        return false;
    }

    /**
     * Enable/Disable read only widget style.
     */
    static void ReadOnlyWigetStyle(bool set) {

        if (set) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        } else {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
    }

private:
    GUIUtils(void) = default;

    ~GUIUtils(void) = default;
};


ImGuiID megamol::gui::gui_generated_uid = 0;


} // namespace gui
} // namespace megamol

#endif // MEGAMOL_GUI_GUIUTILS_INCLUDED
