#ifndef CORE_I_RESPONSE_H_
#define CORE_I_RESPONSE_H_

#include <cstdint>
#include <string_view>

namespace core {
    // IIS-independent mapping of IHttpResponse.
    class IResponse {
    public:
        virtual ~IResponse() = default;

        // errorStatus carries the HRESULT-shaped error code as a plain
        // 32-bit integer so core stays independent from <windows.h>.
        virtual auto SetStatus(
            int statusCode,
            std::string_view reason,
            std::int32_t errorStatus) -> void = 0;

        virtual auto SetHeader(
            std::string_view name,
            std::string_view value) -> void = 0;
    };
}
#endif // CORE_I_RESPONSE_H_
