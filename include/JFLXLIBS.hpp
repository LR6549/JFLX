//
// JFLXLIBS — umbrella header for the whole library ecosystem.
//
// Single entry point: includes the global header of every sub-library, and
// nothing else. Each sub-library header in turn includes its own components,
// so individual components can equally be included on their own:
//   #include <SDL3/TextureHandler.hpp>
//   #include <sorting/sortingAlgorithms.hpp>
//
// Everything lives under the JFLX namespace: classes are addressed directly
// (JFLX::TextRenderer, JFLX::Color, …), grouped free functions sit in their
// sub-library namespace (JFLX::json, JFLX::sort, JFLX::math, …).
//
// Sub-libraries:
//   SDL3            window/renderer configuration + audio, text, textures
//   sorting         sorting algorithms (JFLX::sort)
//   dataStructures  vectors, graphs, trees, generic list/queue/stack
//   mathematics     prime numbers, perlin noise (header-only) + noise
//                   generation and binomial deviation (compiled) (JFLX::math)
//   color           hex <-> RGB(A) conversion (JFLX::color)
//   cryptography    symmetric ciphers
//   positioning     field-of-view checks (JFLX::positioning)
//   strings         string manipulation (JFLX::strings)
//   utilities       logging, current path, runtime flags (JFLX::utilities)
//   network         curl wrapper, Spotify wrapper (optional dependencies)
//   json            JSON file/string I/O, path access, merging
//                   (JFLX::json, optional dependency)
//

#ifndef JFLXLIBS_HPP
#define JFLXLIBS_HPP

#include <SDL3/SDL3.hpp>
#include <sorting/sorting.hpp>
#include <dataStructures/dataStructures.hpp>
#include <mathematics/mathematics.hpp>
#include <color/color.hpp>
#include <cryptography/cryptography.hpp>
#include <positioning/positioning.hpp>
#include <strings/strings.hpp>
#include <utilities/utilities.hpp>
#include <network/network.hpp>
#include <json/json.hpp>

#endif // JFLXLIBS_HPP
