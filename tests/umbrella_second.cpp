//
// Second translation unit for the umbrella test: including <JFLXLIBS.hpp>
// from two TUs must link, i.e. the header-only sub-libraries must not define
// any symbol at namespace or global scope without `inline`/`template`.
//

#include <JFLXLIBS.hpp>

#include <string>
#include <vector>

int umbrellaSecondTranslationUnit() {
    std::string uppered = "jflx";
    JFLX::toUpper(uppered);

    std::vector<int> values {2, 1};
    JFLX::BubbleSort::sort(values);

    return (uppered == "JFLX" && values.front() == 1 && values.back() == 2) ? 1 : 0;
}
