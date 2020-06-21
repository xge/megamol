/*
 * GraphManager.h
 *
 * Copyright (C) 2019 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOL_GUI_GRAPH_GRAPHMANAGER_H_INCLUDED
#define MEGAMOL_GUI_GRAPH_GRAPHMANAGER_H_INCLUDED


#include "Graph.h"

#include "mmcore/CoreInstance.h"
#include "mmcore/Module.h"
#include "mmcore/param/ParamSlot.h"
#include "mmcore/param/AbstractParam.h"
#include "mmcore/utility/plugins/AbstractPluginInstance.h"

#include "utility/plugins/PluginManager.h"


namespace megamol {
namespace gui {
namespace configurator {

// Forward declarations
class GraphManager;

// Types
typedef std::shared_ptr<Graph> GraphPtrType;
typedef std::vector<GraphPtrType> GraphsType;
typedef std::map<megamol::core::param::AbstractParam*, std::shared_ptr<megamol::gui::configurator::Parameter>> ParamInterfaceMapType;


/** ************************************************************************
 * Defines GUI graph manager presentation.
 */
class GraphManagerPresentation {
public:
    // FUNCTIONS --------------------------------------------------------------

    GraphManagerPresentation(void);
    ~GraphManagerPresentation(void);

    void SaveProjectToFile(bool open_popup, GraphManager& inout_graph_manager, GraphStateType& state);

private:
    // VARIABLES --------------------------------------------------------------

    GUIUtils utils;
    megamol::gui::FileUtils file_utils;
    ImGuiID graph_delete_uid;
    
    // FUNCTIONS --------------------------------------------------------------
    
    friend void GraphManager::GUI_Present(GraphStateType& state);
    void Present(GraphManager& inout_graph_manager, GraphStateType& state);    
};


/** ************************************************************************
 * Defines the graph manager.
 */
class GraphManager {
public:

    // VARIABLES --------------------------------------------------------------

    GraphManagerPresentation present;
    
    // FUNCTIONS --------------------------------------------------------------
    
    GraphManager(void);
    ~GraphManager(void);

    ImGuiID AddGraph(void);
    bool DeleteGraph(ImGuiID graph_uid);
    bool GetGraph(ImGuiID graph_uid, GraphPtrType& out_graph_ptr);
    const GraphsType& GetGraphs(void) { return this->graphs; }

    bool LoadModulesCallsStock(const megamol::core::CoreInstance* core_instance);
    inline const ModuleStockVectorType& GetModulesStock(void) { return this->modules_stock; }
    inline const CallStockVectorType& GetCallsStock(void) { return this->calls_stock; }

    ImGuiID LoadUpdateProjectFromCore(ImGuiID graph_uid, megamol::core::CoreInstance* core_instance, ParamInterfaceMapType& inout_param_interface_map);
    
    ImGuiID LoadProjectFromCore(megamol::core::CoreInstance* core_instance);
    bool AddProjectFromCore(ImGuiID graph_uid, megamol::core::CoreInstance* core_instance, bool use_stock);

    ImGuiID LoadAddProjectFromFile(ImGuiID graph_uid, const std::string& project_filename);

    bool SaveProjectToFile(ImGuiID graph_uid, const std::string& project_filename);

    // Presentation ----------------------------------------------------

    void GUI_Present(GraphStateType& state) { this->present.Present(*this, state); }

private:
    // VARIABLES --------------------------------------------------------------

    GraphsType graphs;
    ModuleStockVectorType modules_stock;
    CallStockVectorType calls_stock;
    unsigned int graph_name_uid;

    // FUNCTIONS --------------------------------------------------------------

    bool get_call_stock_data(
        Call::StockCall& call, const std::shared_ptr<const megamol::core::factories::CallDescription> call_desc);
    bool get_module_stock_data(
        Module::StockModule& mod, const std::shared_ptr<const megamol::core::factories::ModuleDescription> mod_desc);

    bool read_project_command_arguments(const std::string& line, size_t arg_count, std::vector<std::string>& out_args);
    ImVec2 project_read_confpos(const std::string& line);
    bool project_separate_name_and_namespace(const std::string& full_name, std::string& name_space, std::string& name);

    bool replace_graph_state(
        const GraphPtrType& graph_ptr, const std::string& in_json_string, std::string& out_json_string);
    bool replace_parameter_gui_state(
        const GraphPtrType& graph_ptr, const std::string& in_json_string, std::string& out_json_string);

    bool parameters_gui_state_from_json_string(const GraphPtrType& graph_ptr, const std::string& in_json_string);
    bool parameters_gui_state_to_json(const GraphPtrType& graph_ptr, nlohmann::json& out_json);

    inline const std::string generate_unique_graph_name(void) {
        return ("Project_" + std::to_string(++graph_name_uid));
    }
};

} // namespace configurator
} // namespace gui
} // namespace megamol

#endif // MEGAMOL_GUI_GRAPH_GRAPHMANAGER_H_INCLUDED
