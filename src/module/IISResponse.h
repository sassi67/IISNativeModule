#ifndef IIS_RESPONSE_H_
#define IIS_RESPONSE_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/Response.h>

namespace iis {
    // Adapter satisfying core::Response over a live IHttpResponse, for
    // the duration of one notification. Tolerates a null IIS pointer at
    // construction; the caller must only invoke the methods when the
    // wrapped pointer is non-null (IISContext enforces this by not
    // handing the adapter out otherwise).
    //
    // The incoming views are forwarded to IIS with .data() rather than
    // being copied into a std::string, which is what keeps this
    // allocation-free. That relies on core::Response's null-termination
    // precondition: every caller passes a view over a string literal.
    class IISResponse final {
    public:
        explicit IISResponse(_In_opt_ IHttpResponse * pResponse) : pResponse_{pResponse} {}

        auto SetStatus(
            int statusCode,
            std::string_view reason,
            std::int32_t errorStatus) -> void;

        auto SetHeader(std::string_view name, std::string_view value) -> void;

    private:
        IHttpResponse * pResponse_;
    };

    // Fails here, with a readable message, if the class ever stops
    // matching the contract the handler template expects.
    static_assert(core::Response<IISResponse>);
}
#endif // IIS_RESPONSE_H_
