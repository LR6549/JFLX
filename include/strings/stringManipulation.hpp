#ifndef JFLX_STRING_MANIPULATION_HPP
#define JFLX_STRING_MANIPULATION_HPP
#include <string>
#include <algorithm>
#include <cctype>

namespace JFLX {
    inline void toLower(std::string& s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }
    inline void toUpper(std::string& s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::toupper(c); });
    }
}
#endif //JFLX_STRING_MANIPULATION_HPP
