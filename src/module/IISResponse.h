#ifndef IIS_RESPONSE_H_
#define IIS_RESPONSE_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/IResponse.h>

namespace iis {
    // Adapter translating a live IHttpResponse into core::IResponse for
    // the duration of one notification. Tolerates a null IIS pointer at
    // construction; the caller must only invoke the methods when the
    // wrapped pointer is non-null (IISContext enforces this by not
    // handing the adapter out otherwise).
    class IISResponse final : public core::IResponse {
    public:
        explicit IISResponse(_In_opt_ IHttpResponse * pResponse) : pResponse_{pResponse} {}

        auto SetStatus(
            int statusCode,
            std::string_view reason,
            std::int32_t errorStatus) -> void override;

        auto SetHeader(std::string_view name, std::string_view value) -> void override;

    private:
        IHttpResponse * pResponse_;
    };
}
#endif // IIS_RESPONSE_H_
