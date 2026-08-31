#include "ModuleHandler.h"

#include <cstddef>
#include <string_view>

#include "AsciiToLower.h"

namespace core::detail {

namespace {

    constexpr std::wstring_view kForbiddenSuffix = L"/forbidden";

}

auto EndsWithForbidden(const RawHttpRequest * rawRequest) -> bool {
    if (rawRequest == nullptr) {
        return false;
    }
    const std::wstring_view path = rawRequest->absPath;
    if (path.length() < kForbiddenSuffix.length()) {
        return false;
    }
    const std::wstring_view tail =
        path.substr(path.length() - kForbiddenSuffix.length());
    for (std::size_t i = 0; i < kForbiddenSuffix.length(); ++i) {
        if (AsciiToLower(tail[i]) != AsciiToLower(kForbiddenSuffix[i])) {
            return false;
        }
    }
    return true;
}

}
