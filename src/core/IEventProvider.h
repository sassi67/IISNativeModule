#ifndef CORE_I_EVENT_PROVIDER_H_
#define CORE_I_EVENT_PROVIDER_H_

#include <cstdint>

namespace core {
    // IIS-independent mapping of IHttpEventProvider.
    class IEventProvider {
    public:
        virtual ~IEventProvider() = default;

        // errorStatus carries the HRESULT-shaped error code as a plain
        // 32-bit integer so core stays independent from <windows.h>.
        virtual auto SetErrorStatus(std::int32_t errorStatus) -> void = 0;
    };
}
#endif // CORE_I_EVENT_PROVIDER_H_
