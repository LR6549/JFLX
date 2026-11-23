//
// jsonFunctionality — load & save JSON files, parse & dump JSON strings,
// dotted-path access ("player.stats.hp") and deep merging, built on
// nlohmann/json. Nothing here throws on bad input: every entry point reports
// failure through its return value (and `log`) instead.
//

#ifndef JFLX_JSONFUNCTIONALITY
#define JFLX_JSONFUNCTIONALITY

#include <charconv>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>
#include <nlohmann/json.hpp>
#include <utilities/logging.hpp>

namespace JFLX::json {
    namespace fs = std::filesystem;

    inline std::string path = fs::current_path().string() + "/";

    namespace detail {

        //* Splits a dotted path ("player.stats.hp") into its segments
        inline std::vector<std::string> splitJsonPath(const std::string& dottedPath) {
            std::vector<std::string> segments;
            std::string segment;
            for (char c : dottedPath) {
                if (c == '.') {
                    segments.push_back(segment);
                    segment.clear();
                } else {
                    segment += c;
                }
            }
            segments.push_back(segment);
            return segments;
        }

        //* Array indices are decimal numbers; everything else is an object key
        inline bool jsonArrayIndex(const std::string& segment, std::size_t& index) {
            if (segment.empty()) {
                return false;
            }
            std::size_t value = 0;
            const char* first = segment.data();
            const char* last = first + segment.size();
            auto [parsed, error] = std::from_chars(first, last, value);
            if (error != std::errc() || parsed != last) {
                return false;
            }
            index = value;
            return true;
        }

        //* Resolves a dotted path to the addressed element, or nullptr when it
        //* does not exist (array segments index into arrays, all others are keys)
        inline const nlohmann::json* resolveJsonPath(const nlohmann::json& j,
                                                     const std::vector<std::string>& segments) {
            const nlohmann::json* current = &j;
            for (const std::string& segment : segments) {
                if (current->is_object()) {
                    auto element = current->find(segment);
                    if (element == current->end()) {
                        return nullptr;
                    }
                    current = &*element;
                } else if (current->is_array()) {
                    std::size_t index = 0;
                    if (!jsonArrayIndex(segment, index) || index >= current->size()) {
                        return nullptr;
                    }
                    current = &(*current)[index];
                } else {
                    return nullptr;
                }
            }
            return current;
        }

    } // namespace detail

    //* Load JSON from file and return as parsed json object
    inline bool loadJson(const std::string& pathToJson, nlohmann::json& j) {
        std::ifstream loadedFile(pathToJson);
        if (!loadedFile.is_open()) {
            utilities::log("Failed to open ", pathToJson, utilities::LOGTYPE::JFLX_ERROR);
            return false;
        }

        try {
            loadedFile >> j;
        } catch (const nlohmann::json::parse_error& e) {
            utilities::log("Failed to parse JSON: ", pathToJson + ": " + e.what(), utilities::LOGTYPE::JFLX_ERROR);
            return false;
        }
        return true;
    }

    //* Save JSON to file with 4-space indentation. The data is written to a
    //* temporary file first and renamed into place, so a failed write can
    //* never destroy an existing file.
    inline bool saveJson(const std::string& pathToJson, const nlohmann::json& j) {
        const std::string temporaryPath = pathToJson + ".tmp";
        {
            std::ofstream file(temporaryPath, std::ios::trunc);
            if (!file.is_open()) {
                utilities::log("JSON COULD NOT BE SAVED: ", ("Json file could not be opened located at: " + temporaryPath), utilities::LOGTYPE::JFLX_ERROR);
                return false;
            }

            file << j.dump(4);
            if (!file.good()) {
                utilities::log("JSON COULD NOT BE SAVED: ", ("Json file could not be written located at: " + temporaryPath), utilities::LOGTYPE::JFLX_ERROR);
                file.close();
                std::error_code cleanupError;
                fs::remove(temporaryPath, cleanupError);
                return false;
            }
        }

        std::error_code renameError;
        fs::rename(temporaryPath, pathToJson, renameError);
        if (renameError) {
            utilities::log("JSON COULD NOT BE SAVED: ", ("Json file could not be replaced located at: " + pathToJson), utilities::LOGTYPE::JFLX_ERROR);
            std::error_code cleanupError;
            fs::remove(temporaryPath, cleanupError);
            return false;
        }
        return true;
    }

    //* Parse JSON from a string instead of a file
    inline bool parseJson(const std::string& content, nlohmann::json& j) {
        try {
            j = nlohmann::json::parse(content);
        } catch (const nlohmann::json::parse_error& e) {
            utilities::log("Failed to parse JSON: ", e.what(), utilities::LOGTYPE::JFLX_ERROR);
            return false;
        }
        return true;
    }

    //* Serialize JSON to a string (indent: spaces per level for pretty
    //* printing; negative values yield compact output)
    inline std::string jsonToString(const nlohmann::json& j, int indent = 4) {
        return j.dump(indent);
    }

    //* Whether a dotted path ("player.inventory.0.name") addresses an element
    inline bool hasJsonPath(const nlohmann::json& j, const std::string& dottedPath) {
        return detail::resolveJsonPath(j, detail::splitJsonPath(dottedPath)) != nullptr;
    }

    //* Reads the element addressed by a dotted path into out
    inline bool getJsonByPath(const nlohmann::json& j, const std::string& dottedPath, nlohmann::json& out) {
        const nlohmann::json* found = detail::resolveJsonPath(j, detail::splitJsonPath(dottedPath));
        if (found == nullptr) {
            return false;
        }
        out = *found;
        return true;
    }

    //* Stores value at a dotted path, creating missing object levels along the
    //* way. Array levels must already exist and are addressed by index.
    inline bool setJsonByPath(nlohmann::json& j, const std::string& dottedPath, const nlohmann::json& value) {
        if (dottedPath.empty()) {
            return false;
        }

        const std::vector<std::string> segments = detail::splitJsonPath(dottedPath);
        if (segments.back().empty()) {
            return false;
        }

        nlohmann::json* current = &j;
        for (std::size_t i = 0; i + 1 < segments.size(); ++i) {
            if (current->is_array()) {
                std::size_t index = 0;
                if (!detail::jsonArrayIndex(segments[i], index) || index >= current->size()) {
                    return false;
                }
                current = &(*current)[index];
                continue;
            }
            if (current->is_null()) {
                *current = nlohmann::json::object();   // created by the level above
            }
            if (!current->is_object()) {
                return false;
            }
            current = &(*current)[segments[i]];
        }

        const std::string& last = segments.back();
        if (current->is_array()) {
            std::size_t index = 0;
            if (!detail::jsonArrayIndex(last, index) || index >= current->size()) {
                return false;
            }
            (*current)[index] = value;
            return true;
        }
        if (current->is_null()) {
            *current = nlohmann::json::object();
        }
        if (!current->is_object()) {
            return false;
        }
        (*current)[last] = value;
        return true;
    }

    //* Deep-merges patch into target: objects merge key by key (recursively),
    //* everything else is replaced by the value from patch.
    inline void mergeJson(nlohmann::json& target, const nlohmann::json& patch) {
        if (target.is_object() && patch.is_object()) {
            for (auto element = patch.begin(); element != patch.end(); ++element) {
                auto existing = target.find(element.key());
                if (existing != target.end()) {
                    mergeJson(*existing, element.value());
                } else {
                    target[element.key()] = element.value();
                }
            }
            return;
        }
        target = patch;
    }

}

#endif
