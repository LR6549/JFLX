//
// Network — global header of the network sub-library.
//
// curlWrapper and spotifyWrapper depend on third-party headers (libcurl and
// nlohmann/json) that JFLXLIBS does not force on its consumers, so both are
// only pulled in when the dependency is actually available. The detected
// state is exposed as JFLX_NETWORK_HAS_CURL / JFLX_NETWORK_HAS_JSON.
//

#ifndef JFLX_NETWORK_HPP
#define JFLX_NETWORK_HPP

#if defined(__has_include)
#  if __has_include(<curl/curl.h>)
#    define JFLX_NETWORK_HAS_CURL 1
#    include <network/curlWrapper.hpp>
#  endif
#  if defined(JFLX_NETWORK_HAS_CURL) && __has_include(<nlohmann/json.hpp>)
#    define JFLX_NETWORK_HAS_JSON 1
#    include <network/spotifyWrapper.hpp>
#  endif
#endif

#endif // JFLX_NETWORK_HPP
