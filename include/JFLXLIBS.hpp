//
// JFLXLIBS — umbrella header for the whole library ecosystem.
//
// Single entry point: includes the global header of every sub-library, and
// nothing else. Each sub-library header in turn includes its own components,
// so individual components can equally be included on their own:
//   #include <SDL3/TextureHandler.hpp>
//   #include <sorting/quickSort.hpp>
//
// Sub-libraries:
//   SDL3            window/renderer configuration + audio, text, textures
//   sorting         sorting algorithms
//   dataStructures  vectors, graphs, trees, generic list/queue/stack
//   mathematics     prime numbers, perlin noise (header-only) + noise
//                   generation and binomial deviation (compiled)
//   cryptography    symmetric ciphers
//   positioning     field-of-view checks
//   strings         string manipulation
//   utilities       logging, current path, runtime flags
//   network         curl wrapper, Spotify wrapper (optional dependencies)
//

#ifndef JFLXLIBS_HPP
#define JFLXLIBS_HPP

#include <SDL3/SDL3.hpp>
#include <sorting/sorting.hpp>
#include <dataStructures/dataStructures.hpp>
#include <mathematics/mathematics.hpp>
#include <cryptography/cryptography.hpp>
#include <positioning/positioning.hpp>
#include <strings/strings.hpp>
#include <utilities/utilities.hpp>
#include <network/network.hpp>

#endif // JFLXLIBS_HPP
