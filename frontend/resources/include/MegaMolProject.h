/*
 * MegaMolProject.h
 *
 * Copyright (C) 2022 by VISUS (Universitaet Stuttgart).
 * Alle Rechte vorbehalten.
 */

#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace megamol {
namespace frontend_resources {

struct MegaMolProject {
    using path = std::filesystem::path;

    struct ProjectAttributes {
        std::string project_name = ""; // name of project directory
        path project_file;
        path project_directory;
    };

    std::optional<ProjectAttributes> attributes = std::nullopt;

    void setProjectFile(path const& file) {
        ProjectAttributes a;
        a.project_file = file;
        a.project_directory = path{file}.remove_filename(); // leaves trailing '/'
        a.project_name = file.parent_path().filename().string();

        attributes = a;
    }
};
} /* end namespace frontend_resources */
} /* end namespace megamol */
