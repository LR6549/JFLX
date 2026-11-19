//
// JSON header test — <json/json.hpp> must compile and link on its own, and
// the jsonFunctionality API must behave: file/string round-trips, dotted-path
// access, deep merging, and failure paths (nothing throws on bad input).
//

#include <json/json.hpp>

#include <cstdio>
#include <filesystem>
#include <string>

#if defined(JFLX_JSON_HAS_NLOHMANN)

namespace {

    bool expect(bool value, const char* what) {
        if (!value) {
            std::fprintf(stderr, "FAIL: %s\n", what);
        }
        return value;
    }

} // namespace

int main() {
    bool ok = true;
    namespace fs = std::filesystem;

    // ── parseJson / jsonToString ───────────────────────────────────────────
    nlohmann::json doc;
    ok &= expect(JFLX::parseJson("{\"player\": {\"stats\": {\"hp\": 7}, \"name\": \"jflx\"}}", doc),
                 "parseJson valid input");
    ok &= expect(!JFLX::parseJson("{not json", doc), "parseJson rejects garbage");
    ok &= expect(JFLX::parseJson(JFLX::jsonToString(doc), doc), "jsonToString round-trip");

    // ── file round-trip: saveJson / loadJson ───────────────────────────────
    const std::string testFile = (fs::temp_directory_path() / "jflx_header_json_test.json").string();
    ok &= expect(JFLX::saveJson(testFile, doc), "saveJson writes the file");
    nlohmann::json loaded = nullptr;
    ok &= expect(JFLX::loadJson(testFile, loaded), "loadJson reads the file");
    ok &= expect(loaded == doc, "file round-trip preserves the document");
    ok &= expect(!JFLX::loadJson("/nonexistent_jflx_test_dir/missing.json", loaded),
                 "loadJson missing file");
    fs::remove(testFile);

    // ── dotted-path access ─────────────────────────────────────────────────
    ok &= expect(JFLX::hasJsonPath(doc, "player.stats.hp"), "hasJsonPath existing");
    ok &= expect(!JFLX::hasJsonPath(doc, "player.stats.mp"), "hasJsonPath missing");
    nlohmann::json value;
    ok &= expect(JFLX::getJsonByPath(doc, "player.stats.hp", value) && value == 7,
                 "getJsonByPath reads the value");
    ok &= expect(!JFLX::getJsonByPath(doc, "player.stats.mp", value), "getJsonByPath missing path");

    ok &= expect(JFLX::setJsonByPath(doc, "player.stats.mp", 3), "setJsonByPath creates levels");
    ok &= expect(JFLX::hasJsonPath(doc, "player.stats.mp"), "setJsonByPath stored the value");
    ok &= expect(!JFLX::setJsonByPath(doc, "", 1), "setJsonByPath rejects empty path");

    nlohmann::json arrayDoc;
    ok &= expect(JFLX::parseJson("{\"items\": [1, 2, 3]}", arrayDoc), "parseJson array document");
    ok &= expect(JFLX::getJsonByPath(arrayDoc, "items.1", value) && value == 2,
                 "getJsonByPath array index");
    ok &= expect(JFLX::setJsonByPath(arrayDoc, "items.1", 20), "setJsonByPath array index");
    ok &= expect(!JFLX::setJsonByPath(arrayDoc, "items.3", 4), "setJsonByPath rejects out of bounds");

    // ── mergeJson ──────────────────────────────────────────────────────────
    nlohmann::json target;
    ok &= expect(JFLX::parseJson("{\"a\": {\"b\": 1, \"c\": 2}, \"keep\": true}", target),
                 "parseJson merge target");
    nlohmann::json patch;
    ok &= expect(JFLX::parseJson("{\"a\": {\"c\": 3, \"d\": 4}}", patch), "parseJson merge patch");
    JFLX::mergeJson(target, patch);
    ok &= expect(target["a"]["b"] == 1, "mergeJson keeps untouched keys");
    ok &= expect(target["a"]["c"] == 3, "mergeJson lets patch values win");
    ok &= expect(target["a"]["d"] == 4, "mergeJson adds new keys");
    ok &= expect(target["keep"] == true, "mergeJson keeps target-only keys");

    if (!ok) {
        std::fputs("JFLXLIBS json header test FAILED\n", stderr);
        return 1;
    }

    std::puts("JFLXLIBS json header test OK");
    return 0;
}

#else // no nlohmann/json on this machine — the umbrella is empty, nothing to test

int main() {
    std::puts("JFLXLIBS json header test SKIPPED (no nlohmann/json)");
    return 0;
}

#endif
