#ifndef IIS_EVENT_PROVIDER_H_
#define IIS_EVENT_PROVIDER_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/IEventProvider.h>

namespace iis {
    // Adapter translating a live IHttpEventProvider into
    // core::IEventProvider for the duration of one notification.
    // Tolerates a null IIS pointer at construction; the caller must
    // only invoke SetErrorStatus when the wrapped pointer is non-null
    // (IISRequestLevelModule enforces this by passing a null provider
    // to the handler instead).
    class IISEventProvider final : public core::IEventProvider {
    public:
        explicit IISEventProvider(_In_opt_ IHttpEventProvider * pProvider) : pProvider_{pProvider} {}

        auto SetErrorStatus(std::int32_t errorStatus) -> void override;

    private:
        IHttpEventProvider * pProvider_;
    };
}
#endif // IIS_EVENT_PROVIDER_H_
