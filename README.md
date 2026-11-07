# JFLXLIBS

Unified CMake build for the JFLX libraries — **six components** compiled into
a **single static archive** (`libJFLXLIBS.a`), plus **eight header-only
sub-libraries**, all reachable through one umbrella header with a clean
public/private header split.

## Entry point

```cpp
#include <JFLXLIBS.hpp>      // every sub-library

// …or one sub-library (each has its own global header)…
#include <SDL3/SDL3.hpp>
#include <sorting/sorting.hpp>

// …or a single component:
#include <SDL3/TextureHandler.hpp>
```

`JFLXLIBS.hpp` includes nothing but sub-library global headers; each of those
in turn includes its own components — no component is pulled in directly by
the umbrella.

## Compiled components

| Component | Public header | Purpose |
|---|---|---|
| `jflx_sdl3_config` | `<SDL3/JFLX_SDL3_CONFIG.hpp>` | Global window/renderer configuration: `JFLX::SDL3::CONFIG::g_window`/`g_renderer` plus `setWindow()`/`setRenderer()`. |
| `jflx_sdl3_audio` | `<SDL3/AudioHandler.hpp>` | Loads and plays sounds/music via SDL3_mixer. Sounds overlap freely on one mixer; music plays on a dedicated track with a true fade-out-then-fade-in transition (completed via the track-stopped callback). Master gain control (`setSoundVolume`/`setMusicVolume` + getters), `pauseMusic`/`resumeMusic`, and per-asset `unloadSound`/`unloadMusic`. Assets live in `<root>/sounds/` (sounds) and `<root>/data/music/` (music), keyed by filename stem. Supports `.mp3`, `.ogg`, `.wav`, `.flac`. |
| `jflx_sdl3_text` | `<SDL3/TextRenderer.hpp>` | Loads TTF fonts and draws UTF-8 text onto an `SDL_Renderer` (by font pointer, index, or name), with optional 8-direction outline. `measureText()` reports the pixel size of a string without rendering. Rendered text textures are cached and reused across frames; `unloadFont()` removes a single font (with cache/index upkeep). |
| `jflx_sdl3_texture` | `<SDL3/TextureHandler.hpp>` | Loads image folders as named/indexed texture layers, slices tilesheets into individual tiles (`loadTileset`), renders with anchor modes, rotation, flip, and colour modulation, and removes single textures via `unloadTexture()` (layer indices re-map automatically). |
| `jflx_math_noise_generation` | `<mathematics/noiseGeneration.hpp>` | Perlin noise over a random gradient grid (namespace `perlinNoise`): grid initialisation, `fade`/`lerp` interpolation, and the scalar and `noiseMap` variants. |
| `jflx_math_bsa` | `<mathematics/bsa.hpp>` | Binomial statistics (namespace `bsa`): µ, sigma from *n/p* or from a known µ, k-sigma intervals, factorial/binomial coefficient, hit chances. |

## Header-only sub-libraries

| Sub-library | Global header | Components |
|---|---|---|
| `sorting` | `<sorting/sorting.hpp>` | `sortingAlgorithms` (`JFLX::MergeSort`, `JFLX::QuickSort`, …) plus `heapSort`, `mergeSort`, `quickSort`, `selectionSort`, `insertionSort`, `gnomeSort`, `bubbleSort`, `bogoSort` |
| `dataStructures` | `<dataStructures/dataStructures.hpp>` | `vectors`, `graph`, `binaryTree`, `genericDataStructures`, `genericList`, `genericQueue`, `genericStack` |
| `mathematics` | `<mathematics/mathematics.hpp>` | `primeNumbers`, `perlinNoise` (header-only; the noise-generation and `bsa` components above are compiled) |
| `cryptography` | `<cryptography/cryptography.hpp>` | `ceasarCypher` |
| `positioning` | `<positioning/positioning.hpp>` | `IFOV` (field-of-view test over `JFLX::vects::Vector2`) |
| `strings` | `<strings/strings.hpp>` | `stringManipulation` |
| `utilities` | `<utilities/utilities.hpp>` | `logging`, `curPath`, `runtimeFlags` |
| `network` | `<network/network.hpp>` | `curlWrapper`, `spotifyWrapper` — included only when `<curl/curl.h>` resp. `<nlohmann/json.hpp>` exist (detected as `JFLX_NETWORK_HAS_CURL` / `JFLX_NETWORK_HAS_JSON`) |

**Dependencies:** the SDL3 components share **SDL3** (+ `SDL3_image`,
`SDL3_mixer`, `SDL3_ttf`) and the header-only `<utilities/logging.hpp>`; the
mathematics components are standard-library only. Every other sub-library is
self-contained — except `network`, which needs **libcurl** (and
**nlohmann/json** for the Spotify wrapper). JFLXLIBS does not link either: a
program calling `cWrap::httpGet()` or `Spotify::SpotifyWrapper` links them
itself.

## Global window & renderer configuration

The SDL3 window/renderer are managed centrally instead of per module:

```cpp
#include <JFLXLIBS.hpp>

SDL_Window*   window   = /* create SDL3 window */;
SDL_Renderer* renderer = /* create SDL3 renderer */;

JFLX::SDL3::CONFIG::setWindow(window);
JFLX::SDL3::CONFIG::setRenderer(renderer);

JFLX::SDL3::TextureHandler textures;   // uses CONFIG::g_renderer
JFLX::SDL3::TextRenderer   text;       // ditto, unless given one explicitly
```

- A per-instance renderer (`TextureHandler(SDL_Renderer*)`, `setRenderer()`,
  `TextRenderer::setDefaultRenderer()`) still wins over the global one, so
  existing call sites keep working.
- `setWindow(nullptr)` / `setRenderer(nullptr)` clear the configuration — call
  them before destroying the SDL resources so the globals cannot dangle.
- `g_window` / `g_renderer` are borrowed: JFLXLIBS never owns or destroys them.
- `AudioHandler` needs neither a window nor a renderer.

## Directory structure

```text
JFLXLIBS/
├── CMakeLists.txt              # global build: 6 object libs → libJFLXLIBS.a
├── cmake/
│   └── JFLXLIBSConfig.cmake.in # install/export package template
├── include/                    # PUBLIC headers only, one folder per sub-library
│   │                           #   each folder holds <sub-library>.hpp (global
│   │                           #   header) + its components
│   ├── JFLXLIBS.hpp            # umbrella: includes the 9 global headers
│   ├── SDL3/                   # compiled: config, audio, text, texture
│   │   ├── SDL3.hpp
│   │   ├── JFLX_SDL3_CONFIG.hpp
│   │   ├── AudioHandler.hpp
│   │   ├── TextRenderer.hpp
│   │   └── TextureHandler.hpp
│   ├── mathematics/            # + compiled: noiseGeneration, bsa
│   │   ├── mathematics.hpp
│   │   ├── primeNumbers.hpp
│   │   ├── perlinNoise.hpp
│   │   ├── noiseGeneration.hpp
│   │   └── bsa.hpp
│   ├── sorting/                # sorting.hpp + sortingAlgorithms + 8 sorts
│   ├── dataStructures/         # dataStructures.hpp + 7 component headers
│   ├── cryptography/           # cryptography.hpp, ceasarCypher.hpp
│   ├── positioning/            # positioning.hpp, IFOV.hpp
│   ├── strings/                # strings.hpp, stringManipulation.hpp
│   ├── utilities/              # utilities.hpp, logging/curPath/runtimeFlags
│   └── network/                # network.hpp, curlWrapper/spotifyWrapper
├── src/                        # implementation files (private)
│   ├── JFLX_SDL3_CONFIG.cpp
│   ├── AudioHandler.cpp
│   ├── TextRenderer.cpp
│   ├── TextureHandler.cpp
│   ├── noiseGeneration.cpp
│   └── bsa.cpp
├── tests/                      # link + header self-containment tests
│   ├── CMakeLists.txt
│   ├── link_smoke.cpp
│   ├── header_{audio,text,texture}.cpp
│   ├── header_umbrella.cpp     # umbrella + every sub-library, two TUs
│   └── umbrella_second.cpp     #   (second TU → catches duplicate symbols)
├── build/                      # Release build tree (generated)
└── build-debug/                # Debug build tree (generated)
```

Implementation details (iteration helpers, cache key types, `std::filesystem`
usage inside `src/`) never leak into the SDL3 public headers — each public
header is self-contained and compiles standalone.

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
- `header_umbrella` — includes `<JFLXLIBS.hpp>` from **two** translation units,
  exercises every sub-library (including the compiled `bsa`/noise-generation
  symbols) and the global SDL3 configuration, so both reachability and
  multiple-definition freedom are checked.

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
   - `#include "AudioHandler.hpp"` → `#include <SDL3/AudioHandler.hpp>`
   - `#include "TextRenderer.hpp"` → `#include <SDL3/TextRenderer.hpp>`
   - `#include "textureHandler.hpp"` → `#include <SDL3/TextureHandler.hpp>`
   - the previous `JFLX/` prefix is gone: `<JFLX/SDL3/…>` → `<SDL3/…>`,
     `<JFLX/logging.hpp>` → `<utilities/logging.hpp>`.
     `<JFLXLIBS.hpp>` now includes one global header per sub-library instead
     of listing the individual SDL3 headers.
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
   the SDL3 headers (it is only reintroduced by the integrated
   `<utilities/curPath.hpp>`); define it locally in your own code if you used
   it elsewhere.
6. Per-library `CMakeLists.txt` files were removed; build from the root only.
7. `playMusic()` now starts the new track **after** the previous one has
   finished its fade-out (as documented), instead of switching immediately.
   `stopMusic()`/`pauseMusic()` cancel or defer a queued switch accordingly.
8. **Renderer fallback** — `TextureHandler` and `TextRenderer` fall back to
   `JFLX::SDL3::CONFIG::g_renderer` when they were not given one. Code that
   relied on "no renderer set" failing now draws with the global renderer (or
   fails only when the global one is unset too).

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

## Integrated payload (`LIBRARIES TO ADD TO THE MAIN PROJECT/`)

The staged payload is kept untouched in the repository; these files were taken
over:

| Taken from the staging folder | Goes to |
|---|---|
| `sortingAlgorithms.hpp`, `sortingAlgorythms/<n>_<algo>/<algo>.hpp` | `include/sorting/` |
| `vectors.hpp`, `dataStructures/**` (`graph`, `binaryTree`, `generic*`) | `include/dataStructures/` |
| `perlinNoise.hpp`, `mathematics/primeNumbers/primeNumbers.hpp` | `include/mathematics/` |
| `mathematics/noiseGeneration/perlinNoise/perlinNoise.cpp` | `src/noiseGeneration.cpp` |
| `mathematics/BinomischeStadartAbweichung/bsa.cpp` | `src/bsa.cpp` |
| `cryptography/symetrical/ceasarCypher/ceasarCypher.hpp` | `include/cryptography/` |
| `positioning/isInFOVOrNot/IFOV.hpp` | `include/positioning/` |
| `stringManipulation.hpp` | `include/strings/` |
| `logging.hpp`, `curPath.hpp`, `runtimeFlags/rtf.hpp` | `include/utilities/` |
| `curlWrapper.hpp`, `Spotify/spotifyWrapper.hpp` | `include/network/` |

**Deliberately not integrated**

- Binaries and data: every `*.exe`, `lightingTest/` (experiment incl. its STB
  copy, `lightMap.png`, `map.txt`), `.InfosOnCppAndJFLX/` language samples,
  the sorting `benchMarks/` and per-algorithm `benchMarkTime.cpp` files (all
  define `main()`), and the `CURRENTLY UNCOMPLETE` marker file.
- Exact duplicates: `binarySearchTree/binarySearchTree.hpp` (same `BINARYTREE`
  guard and namespace as the integrated `binaryTree.hpp`), and
  `dataStructures/vectors/vector2.hpp` + `positioning/isInFOVOrNot/vectors.hpp`
  (same `VECTS` guard as the canonical `include/dataStructures/vectors.hpp`,
  but the old `vects` namespace `IFOV.hpp` does not use).

**Fixes applied to the integrated sources** (they did not compile as delivered)

- `graph.hpp`: missing include guard; `Vertex::isMarked()` returned the
  function itself; the second `allVerticesMarked` overload is the *edge* query
  and is now `allEdgesMarked`; `getEdges(Vertex)` searched a `vector<Edge>`
  with a `Vertex`; constructor init order and `&&`/`||` parentheses.
- `sortingAlgorithms.hpp`: unqualified `swap(...)` → `std::swap` (the
  templates would not instantiate) and two signed/unsigned size comparisons.
- `heapSort.hpp` missing `<vector>`; `ceasarCypher.hpp` missing
  `<climits>`/`<cstdlib>`/`<string>`.
- `primeNumbers.hpp` / `perlinNoise.hpp`: namespace-scope functions marked
  `inline` (they are included by more than one translation unit);
  `randomChoice` index arithmetic widened to `unsigned long long`.
- `runtimeFlags.hpp` (guard + `inline` globals) and `curlWrapper.hpp`
  (`inline httpGet`).
- `IFOV.hpp`: `#include <JFLX/vectors.hpp>` → `<dataStructures/vectors.hpp>`.
- `bsa.cpp`: the two `calculateSigma` and two `calculateSigmaIntervall`
  definitions had identical parameter lists, so the µ-based variants are now
  `calculateSigmaWithMean()` / `calculateSigmaIntervallWithMean()`.
- `perlinNoise.cpp`: `struct Gradient` moved into
  `<mathematics/noiseGeneration.hpp>`, the pointless header guard dropped.

## Open points (need approval)

1. `perlinNoise::perlinNoise(noiseMap, seed, smoothness)` takes the map **by
   value** — it fills a copy and the caller's grid stays untouched (asserted
   by `header_umbrella`). Switching it to a reference is a one-word change.
2. `bsa::calculateHitChance(n, p, k)` uses `p²·(1-p)²` instead of
   `pᵏ·(1-p)ⁿ⁻ᵏ`, and `bsa::calculateHitChanceForInterval(...)` sums that same
   constant `k` instead of stepping `i` across `[min, max)`. Both look like
   unfinished formulas; fixing them changes numeric results.
3. `graph.hpp` behaviour: `Edge::operator==` compares `vertexA` twice (never
   `vertexB`), `removeVertex`/`removeEdge` erase through a `const_iterator`
   and keep using it, `getVertex`/`getEdge` fall off the end when nothing is
   found, and `setAllVertexMarks`/`setAllEdgeMarks` loop by value so nothing
   gets marked.
4. `network` stays header-only — linking libcurl/nlohmann-json into the
   archive would add a hard dependency for every consumer.
5. `CONFIG::g_window` has no consumer yet; a window/event component would need
   approval.
6. `include/SDL3/` merges into SDL3's own include directory when installed (no
   name collisions today); a `JFLX_SDL3/` folder would avoid the merge.

## Note on `buildConfig.cmake`

The shared module (`CompilerUndScripts/CMake/Modules/buildConfig.cmake`) had
several `*_DEPS` values written as file names instead of linker names
(`libwebp` → `-llibwebp`, stale `libpng18`, `libzmq`, `libcrypto`/`libssl`)
and was missing the static backends of freetype (`bz2`, brotli) and webp
(`sharpyuv`). These were fixed at the source (approved), so every project
using `buildConfig` links correctly; the root `CMakeLists.txt` now consumes
the dependency lists verbatim.

Behaviour of all other public functions is unchanged.
