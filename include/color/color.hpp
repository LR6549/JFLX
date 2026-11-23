//
// Color — global header of the color sub-library.
//
// JFLX::Color is a plain RGBA color (8 bits per channel) that can be built
// from channel values or from a hex string. The free functions in
// JFLX::color convert between hex strings ("#rrggbb") and RGB(A) values.
//
// Accepted hex forms (leading '#' optional, any case):
//   "#RGB", "#RGBA", "#RRGGBB", "#RRGGBBAA"
// A missing alpha channel is treated as fully opaque (255).
// Malformed input throws std::invalid_argument.
//

#ifndef JFLX_COLOR_HPP
#define JFLX_COLOR_HPP

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace JFLX {

    struct Color;

    namespace color {
        //* Converts one hex digit ('0'-'9', 'a'-'f', 'A'-'F') to its value.
        //* Throws std::invalid_argument on any other character.
        std::uint8_t hexDigitToValue(char digit);

        //* Converts a hex string ("#rgb", "#rgba", "#rrggbb", "#rrggbbaa";
        //* '#' optional, any case) to a Color. A missing alpha channel
        //* becomes 255. Throws std::invalid_argument on malformed input.
        Color hexToRgb(std::string_view hex);

        //* RGB channel values to a lowercase hex string ("#rrggbb").
        std::string rgbToHex(std::uint8_t r, std::uint8_t g, std::uint8_t b);

        //* RGBA channel values to a lowercase hex string ("#rrggbbaa").
        std::string rgbToHex(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);

        //* Color to a lowercase hex string: "#rrggbb", or "#rrggbbaa" when
        //* includeAlpha is set.
        std::string rgbToHex(const Color& c, bool includeAlpha = false);

        //* Linearly blends between two colors and returns the result as a new
        //* Color: amount 0.0 is @p a, amount 1.0 is @p b (per channel, alpha
        //* included, rounded). Values outside [0.0, 1.0] are clamped.
        Color blend(const Color& a, const Color& b, double amount);

        //* Same as blend(), with the blend degree given as a percentage:
        //* 0 is @p a, 100 is @p b. Values outside [0, 100] are clamped.
        Color blendPercent(const Color& a, const Color& b, double percent);
    } // namespace color

    //* An RGBA color, 8 bits per channel. Constructible from channel values
    //* or from a hex string; toHex() converts back to a hex string.
    struct Color {
        std::uint8_t r = 0;
        std::uint8_t g = 0;
        std::uint8_t b = 0;
        std::uint8_t a = 255;

        Color() = default;

        constexpr Color(std::uint8_t pR, std::uint8_t pG, std::uint8_t pB, std::uint8_t pA = 255)
            : r(pR), g(pG), b(pB), a(pA) {}

        //* Builds the color from a hex string ("#rgb", "#rgba", "#rrggbb"
        //* or "#rrggbbaa"; '#' optional). Throws std::invalid_argument on
        //* malformed input.
        explicit Color(std::string_view hex)
            : Color(color::hexToRgb(hex)) {}

        //* "#rrggbb", or "#rrggbbaa" when includeAlpha is set.
        std::string toHex(bool includeAlpha = false) const {
            return color::rgbToHex(*this, includeAlpha);
        }

        bool operator==(const Color&) const = default;
    };

    namespace color {

        inline std::uint8_t hexDigitToValue(char digit) {
            if (digit >= '0' && digit <= '9') return static_cast<std::uint8_t>(digit - '0');
            if (digit >= 'a' && digit <= 'f') return static_cast<std::uint8_t>(digit - 'a' + 10);
            if (digit >= 'A' && digit <= 'F') return static_cast<std::uint8_t>(digit - 'A' + 10);
            throw std::invalid_argument(std::string("color: '") + digit + "' is not a hex digit");
        }

        inline Color hexToRgb(std::string_view hex) {
            if (!hex.empty() && hex.front() == '#') {
                hex.remove_prefix(1);
            }
            if (hex.size() != 3 && hex.size() != 4 && hex.size() != 6 && hex.size() != 8) {
                throw std::invalid_argument("color: hex string must have 3, 4, 6 or 8 digits");
            }

            // Short forms "#rgb"/"#rgba" expand each digit ("a" -> "aa").
            std::string expanded;
            std::string_view digits = hex;
            if (hex.size() == 3 || hex.size() == 4) {
                expanded.reserve(hex.size() * 2);
                for (char digit : hex) {
                    expanded.push_back(digit);
                    expanded.push_back(digit);
                }
                digits = expanded;
            }

            Color result;
            result.r = static_cast<std::uint8_t>((hexDigitToValue(digits[0]) << 4) | hexDigitToValue(digits[1]));
            result.g = static_cast<std::uint8_t>((hexDigitToValue(digits[2]) << 4) | hexDigitToValue(digits[3]));
            result.b = static_cast<std::uint8_t>((hexDigitToValue(digits[4]) << 4) | hexDigitToValue(digits[5]));
            result.a = digits.size() == 8
                           ? static_cast<std::uint8_t>((hexDigitToValue(digits[6]) << 4) | hexDigitToValue(digits[7]))
                           : 255;
            return result;
        }

        inline std::string rgbToHex(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
            return rgbToHex(Color(r, g, b), false);
        }

        inline std::string rgbToHex(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
            return rgbToHex(Color(r, g, b, a), true);
        }

        inline std::string rgbToHex(const Color& c, bool includeAlpha) {
            static constexpr char digits[] = "0123456789abcdef";
            auto appendByte = [](std::string& out, std::uint8_t value) {
                out.push_back(digits[value >> 4]);
                out.push_back(digits[value & 0x0F]);
            };

            std::string result;
            result.reserve(includeAlpha ? 9 : 7);
            result.push_back('#');
            appendByte(result, c.r);
            appendByte(result, c.g);
            appendByte(result, c.b);
            if (includeAlpha) {
                appendByte(result, c.a);
            }
            return result;
        }

        inline Color blend(const Color& a, const Color& b, double amount) {
            if (amount < 0.0) amount = 0.0;
            if (amount > 1.0) amount = 1.0;

            auto blendChannel = [amount](std::uint8_t from, std::uint8_t to) {
                return static_cast<std::uint8_t>(std::lround(from + (to - from) * amount));
            };
            return Color(blendChannel(a.r, b.r),
                         blendChannel(a.g, b.g),
                         blendChannel(a.b, b.b),
                         blendChannel(a.a, b.a));
        }

        inline Color blendPercent(const Color& a, const Color& b, double percent) {
            return blend(a, b, percent / 100.0);
        }

    } // namespace color
} // namespace JFLX

#endif // JFLX_COLOR_HPP
