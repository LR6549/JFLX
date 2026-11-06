# JFLXLIBS

Unified CMake build for the JFLX SDL3 helper libraries — three independent
components compiled into a **single static archive** (`libJFLXLIBS.a`) with a
clean public/private header split.

## Libraries

| Component | Public header | Purpose |
|---|---|---|
| `jflx_sdl3_audio` | `<JFLX/SDL3/AudioHandler.hpp>` | Loads and plays sounds/music via SDL3_mixer. Sounds overlap freely on one mixer; music plays on a dedicated track with a true fade-out-then-fade-in transition (completed via the track-stopped callback). Master gain control (`setSoundVolume`/`setMusicVolume` + getters), `pauseMusic`/`resumeMusic`, and per-asset `unloadSound`/`unloadMusic`. Assets live in `<root>/sounds/` (sounds) and `<root>/data/music/` (music), keyed by filename stem. Supports `.mp3`, `.ogg`, `.wav`, `.flac`. |
| `jflx_sdl3_text` | `<JFLX/SDL3/TextRenderer.hpp>` | Loads TTF fonts and draws UTF-8 text onto an `SDL_Renderer` (by font pointer, index, or name), with optional 8-direction outline. `measureText()` reports the pixel size of a string without rendering. Rendered text textures are cached and reused across frames; `unloadFont()` removes a single font (with cache/index upkeep). |
| `jflx_sdl3_texture` | `<JFLX/SDL3/TextureHandler.hpp>` | Loads image folders as named/indexed texture layers, slices tilesheets into individual tiles (`loadTileset`), renders with anchor modes, rotation, flip, and colour modulation, and removes single textures via `unloadTexture()` (layer indices re-map automatically). |

Convenience umbrella header (all three):

```cpp
#include <JFLXLIBS.hpp>
```

**Dependencies between components: none** — the three libraries are fully
independent and are only bundled into one archive. Externally they share
**SDL3** (+ `SDL3_mixer`, `SDL3_ttf`, `SDL3_image`); during compilation they
also use the header-only `<JFLX/logging.hpp>`.

## Directory structure

```text
JFLXLIBS/
├── CMakeLists.txt              # global build: 3 object libs → libJFLXLIBS.a
├── cmake/
│   └── JFLXLIBSConfig.cmake.in # install/export package template
├── include/                    # PUBLIC headers only
│   ├── JFLXLIBS.hpp            # umbrella header
│   └── JFLX/SDL3/
│       ├── AudioHandler.hpp
│       ├── TextRenderer.hpp
│       └── TextureHandler.hpp
├── src/                        # implementation files (private)
│   ├── AudioHandler.cpp
│   ├── TextRenderer.cpp
│   └── TextureHandler.cpp
├── tests/                      # link + header self-containment tests
│   ├── CMakeLists.txt
│   ├── link_smoke.cpp
│   └── header_{audio,text,texture}.cpp
├── build/                      # Release build tree (generated)
└── build-debug/                # Debug build tree (generated)
```

Internal details (`<JFLX/logging.hpp>`, `std::filesystem`, iteration helpers,
cache key types) never appear in public headers — each public header is
self-contained and compiles standalone.

## Building

The root `CMakeLists.txt` is the only build entry point. It reuses the
external `buildConfig.cmake` module (platform roots, SDL dependency lists) —
the same module the per-library builds used.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Debug:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel
```

Result: **`build/libJFLXLIBS.a`** (Debug: `build-debug/libJFLXLIBS.a`).

### Build options

| Option | Default | Meaning |
|---|---|---|
| `CMAKE_BUILD_TYPE` | — | `Release` (`-O3 -DNDEBUG`) or `Debug` (`-g`) |
| `JFLXLIBS_BUILD_TESTS` | `OFF` | Build the smoke/header tests (`ctest` in the build dir) |
| `JFLXLIBS_WARNINGS` | `ON` | `-Wall -Wextra` (MSVC: `/W4`) on JFLXLIBS components only |
| `JFLXLIBS_WARNINGS_AS_ERRORS` | `OFF` | `-Werror` / `/WX` |

Components are built as `POSITION_INDEPENDENT_CODE` object libraries, so the
archive can also be linked into shared libraries. MSVC builds use the static
runtime (`MultiThreaded[Debug]`), matching the previous per-library setup.

### Tests

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DJFLXLIBS_BUILD_TESTS=ON
cmake --build build --parallel
cd build && ctest --output-on-failure
```

- `link_smoke` — an application that uses **only** the public headers and the
  archive, exercises the query APIs, and exits 0.
- `header_audio` / `header_text` / `header_texture` — prove each public header
  compiles and links on its own.

## Using the archive

### From CMake (recommended)

```cmake
add_executable(my_game main.cpp)
target_link_libraries(my_game PRIVATE JFLXLIBS)   # from this source tree
# or, after installation:
# find_package(JFLXLIBS REQUIRED)
# target_link_libraries(my_game PRIVATE JFLXLIBS::JFLXLIBS)
```

The `JFLXLIBS` target propagates C++20, the `include/` directory, the SDL
include roots, and all SDL link dependencies (they are listed twice on the
link line to resolve static-archive back-edges such as freetype → png).

### Manually with g++

```bash
g++ main.cpp -std=c++20 \
    -I./include \
    -I/path/to/vcpkg/installed/x64-linux/include \
    -L./build -lJFLXLIBS \
    -L/path/to/vcpkg/installed/x64-linux/lib \
    -lSDL3 -lSDL3_image -lSDL3_mixer -lSDL3_ttf \
    -lpng -lwebp -lwebpdemux -lwebpmux -ltiff -lz -lsharpyuv \
    -lfreetype -lharfbuzz -lbz2 -lbrotlidec -lbrotlicommon \
    -logg -lopus -lopusfile -lvorbis -lvorbisfile \
    -lasound -lpulse -lX11 -lpthread -ldl -lm \
    -o main
```

(Exact codec/system flags depend on the platform; the flags CMake generated
for the test executable are recorded in
`build/tests/CMakeFiles/jflx_link_smoke.dir/link.txt`.)

### Installing

```bash
cmake --install build --prefix <prefix>
```

installs `include/` (public headers), `lib*/libJFLXLIBS.a`, and a CMake
package (`find_package(JFLXLIBS)`) under `<prefix>`.

## Migration notes (breaking changes vs. the per-library layout)

1. **Include paths** — headers moved:
   - `#include "AudioHandler.hpp"` → `#include <JFLX/SDL3/AudioHandler.hpp>`
   - `#include "TextRenderer.hpp"` → `#include <JFLX/SDL3/TextRenderer.hpp>`
   - `#include "textureHandler.hpp"` → `#include <JFLX/SDL3/TextureHandler.hpp>`
2. **`AudioHandler::loadSounds(path)`** now scans `<path>/sounds/` (as the
   documentation always stated) instead of `path` itself, and both
   `loadSounds`/`loadMusic` return `true` only when at least one file loaded.
   `loadMusic(path)` still scans `<path>/data/music/`.
3. **`TextRenderer` gained `clearTextCache()`** — call it before destroying an
   `SDL_Renderer` that cached text textures may still reference.
4. **`TextureHandler::loadTileset()`** is new (approved addition): slices a
   tilesheet into layers named `<stem>_<n>`, row-major, appended after
   existing layers.
5. The global `namespace fs = std::filesystem;` alias is no longer exported by
   public headers; define it locally in your own code if you used it.
6. Per-library `CMakeLists.txt` files were removed; build from the root only.
7. `playMusic()` now starts the new track **after** the previous one has
   finished its fade-out (as documented), instead of switching immediately.
   `stopMusic()`/`pauseMusic()` cancel or defer a queued switch accordingly.

## Approved additions (new API)

- `TextRenderer::measureText(text, fontSize, font|index|name, width, height)`
  — pixel size of a string, no renderer required.
- `TextRenderer::unloadFont(index|name)` — closes one font, drops its cached
  textures, re-maps name/index bookkeeping.
- `AudioHandler::setSoundVolume/setMusicVolume/getSoundVolume/getMusicVolume`
  — master gain of the SFX/music mixers (0 = silent, 1 = default, negative
  clamped to 0).
- `AudioHandler::pauseMusic()/resumeMusic()` — pause/resume the music track.
- `AudioHandler::unloadSound(name)/unloadMusic(name)` — free a single asset;
  playing audio finishes safely (SDL_mixer refcounts it).
- `TextureHandler::unloadTexture(layer|name)` — free a single texture; later
  layer indices shift down and name→layer entries re-map.
- `TextureHandler::loadTileset(...)` and `TextRenderer::clearTextCache()`
  (see above).

## Note on `buildConfig.cmake`

The shared module (`CompilerUndScripts/CMake/Modules/buildConfig.cmake`) had
several `*_DEPS` values written as file names instead of linker names
(`libwebp` → `-llibwebp`, stale `libpng18`, `libzmq`, `libcrypto`/`libssl`)
and was missing the static backends of freetype (`bz2`, brotli) and webp
(`sharpyuv`). These were fixed at the source (approved), so every project
using `buildConfig` links correctly; the root `CMakeLists.txt` now consumes
the dependency lists verbatim.

Behaviour of all other public functions is unchanged.
