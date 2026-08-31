#ifndef CORE_EVENT_PROVIDER_H_
#define CORE_EVENT_PROVIDER_H_

#include <concepts>
#include <cstdint>

namespace core {
    // Compile-time counterpart of IHttpEventProvider.
    //
    // errorStatus carries the HRESULT-shaped error code as a plain 32-bit
    // integer so core stays independent from <windows.h>.
    template <typename T>
    concept EventProvider = requires(T & provider) {
        { provider.SetErrorStatus(std::int32_t{}) } -> std::same_as<void>;
    };
}
#endif // CORE_EVENT_PROVIDER_H_
