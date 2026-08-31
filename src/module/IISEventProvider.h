#ifndef IIS_EVENT_PROVIDER_H_
#define IIS_EVENT_PROVIDER_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/EventProvider.h>

namespace iis {
    // Adapter satisfying core::EventProvider over a live
    // IHttpEventProvider, for the duration of one notification.
    // Tolerates a null IIS pointer at construction; the caller must only
    // invoke SetErrorStatus when the wrapped pointer is non-null
    // (IISRequestLevelModule enforces this by passing a null provider to
    // the handler instead).
    class IISEventProvider final {
    public:
        explicit IISEventProvider(_In_opt_ IHttpEventProvider * pProvider) : pProvider_{pProvider} {}

        auto SetErrorStatus(std::int32_t errorStatus) -> void;

    private:
        IHttpEventProvider * pProvider_;
    };

    // Fails here, with a readable message, if the class ever stops
    // matching the contract the handler template expects.
    static_assert(core::EventProvider<IISEventProvider>);
}
#endif // IIS_EVENT_PROVIDER_H_
