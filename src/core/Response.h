#ifndef CORE_RESPONSE_H_
#define CORE_RESPONSE_H_

#include <concepts>
#include <cstdint>
#include <string_view>

namespace core {
    // Compile-time counterpart of IHttpResponse.
    //
    // The string_view parameters must be NULL-TERMINATED at the character
    // one past their end: the IIS adapter forwards .data() straight to
    // the server's PCSTR-taking API rather than materializing a
    // std::string, which is what keeps the hot path allocation-free. All
    // current call sites pass views over string literals, which satisfy
    // this. Do not pass a view into the middle of a larger buffer.
    //
    // errorStatus carries the HRESULT-shaped error code as a plain 32-bit
    // integer so core stays independent from <windows.h>.
    template <typename T>
    concept Response = requires(T & response) {
        { response.SetStatus(int{}, std::string_view{}, std::int32_t{}) } -> std::same_as<void>;
        { response.SetHeader(std::string_view{}, std::string_view{}) } -> std::same_as<void>;
    };
}
#endif // CORE_RESPONSE_H_
