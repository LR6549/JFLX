//
// Link smoke test — uses ONLY the public headers and the generated
// libJFLXLIBS.a archive. Exercises query paths that need no audio device
// or GPU, proving the archive is consumable by an external application.
//

#include <JFLXLIBS.hpp>

#include <cstdio>

namespace {

    bool expectFalse(bool value, const char* what) {
        if (value) {
            std::fprintf(stderr, "FAIL: expected false: %s\n", what);
            return false;
        }
        return true;
    }

} // namespace

int main() {
    JFLX::SDL3::AudioHandler audio;
    JFLX::SDL3::TextRenderer text;
    JFLX::SDL3::TextureHandler textures;

    bool ok = true;

    // AudioHandler public API
    ok &= expectFalse(audio.hasSound("jflx_missing_sound"), "hasSound(missing)");
    ok &= expectFalse(audio.hasMusic("jflx_missing_music"), "hasMusic(missing)");
    ok &= expectFalse(audio.loadSounds("/nonexistent_jflx_test_root"), "loadSounds(missing dir)");

    // TextRenderer public API
    ok &= expectFalse(text.getFont(-1) != nullptr, "getFont(-1)");
    ok &= expectFalse(text.getFont(999) != nullptr, "getFont(999)");
    ok &= expectFalse(text.getFont("jflx_missing_font") != nullptr, "getFont(missing name)");

    // TextRenderer measurement / font unloading (failure paths)
    float w = 0.f, h = 0.f;
    ok &= expectFalse(text.measureText("x", 12.f, static_cast<TTF_Font*>(nullptr), w, h),
                      "measureText(null font)");
    ok &= expectFalse(text.measureText("x", 12.f, -1, w, h), "measureText(bad index)");
    ok &= expectFalse(text.unloadFont(-1), "unloadFont(-1)");
    ok &= expectFalse(text.unloadFont("jflx_missing_font"), "unloadFont(missing name)");

    // AudioHandler volume / unloading (failure paths)
    (void) audio.setSoundVolume(-1.f);   // clamped, must not crash
    (void) audio.setMusicVolume(-1.f);
    ok &= expectFalse(audio.getSoundVolume() < 0.f, "getSoundVolume() negative");
    ok &= expectFalse(audio.getMusicVolume() < 0.f, "getMusicVolume() negative");
    (void) audio.pauseMusic();           // no-op when nothing plays
    (void) audio.resumeMusic();
    ok &= expectFalse(audio.unloadSound("jflx_missing_sound"), "unloadSound(missing)");
    ok &= expectFalse(audio.unloadMusic("jflx_missing_music"), "unloadMusic(missing)");

    // TextureHandler public API
    ok &= expectFalse(textures.exists("jflx_missing_texture"), "exists(missing)");
    ok &= expectFalse(textures.layers() != 0, "layers() empty");
    ok &= expectFalse(textures.getTexture(-1) != nullptr, "getTexture(-1)");
    ok &= expectFalse(textures.getTexture(0) != nullptr, "getTexture(0) on empty");
    ok &= expectFalse(textures.getTextureLayer("jflx_missing") != -1, "getTextureLayer(missing)");
    ok &= expectFalse(textures.unloadTexture(-1), "unloadTexture(-1)");
    ok &= expectFalse(textures.unloadTexture("jflx_missing"), "unloadTexture(missing)");
    ok &= expectFalse(textures.loadTextureFolder("/nonexistent_jflx_test_dir"),
                      "loadTextureFolder(missing dir)");
    ok &= expectFalse(textures.renderTexture(0, 0.f, 0.f), "renderTexture(empty)");

    // Public enum type is reachable
    const auto mode = JFLX::SDL3::renderMode::JFLX_RENDER_CENTERED;
    (void)mode;

    if (!ok) {
        std::fputs("JFLXLIBS link smoke test FAILED\n", stderr);
        return 1;
    }

    std::puts("JFLXLIBS link smoke test OK");
    return 0;
}
