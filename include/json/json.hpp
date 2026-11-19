//
// JSON — global header of the json sub-library.
//
// jsonFunctionality depends on a third-party header (nlohmann/json) that
// JFLXLIBS does not force on its consumers, so it is only pulled in when the
// dependency is actually available. The detected state is exposed as
// JFLX_JSON_HAS_NLOHMANN.
//

#ifndef JFLX_JSON_HPP
#define JFLX_JSON_HPP

#if defined(__has_include)
#  if __has_include(<nlohmann/json.hpp>)
#    define JFLX_JSON_HAS_NLOHMANN 1
#    include <json/jsonFunctionality.hpp>
#  endif
#endif

#endif // JFLX_JSON_HPP
