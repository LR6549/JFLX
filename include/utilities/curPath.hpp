//
// Created by lenny on 02.05.2026.
//

#ifndef GIFARCHIVE_CURPATH_HPP
#define GIFARCHIVE_CURPATH_HPP

#include <string>
#include <filesystem>
namespace fs = std::filesystem;

namespace JFLX {
    inline std::string curPath = fs::current_path().string();

    inline std::string getCurrentPath() {
        return curPath;
    }

    inline void setCurrentPath(const std::string& path) {
        if (!fs::exists(path)) {
            fs::create_directory(path);
        }
        curPath = path;
    }
}

#endif //GIFARCHIVE_CURPATH_HPP
