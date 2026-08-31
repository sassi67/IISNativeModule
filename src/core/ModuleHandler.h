#ifndef CORE_MODULE_HANDLER_H_
#define CORE_MODULE_HANDLER_H_

#include <cstdint>
#include <string_view>

#include "Context.h"
#include "EventProvider.h"
#include "Verdict.h"

namespace core {

    namespace detail {

        // Constants stay in the header because the OnBeginRequest
        // template below is expanded in the caller's translation unit and
        // has to see them.
        inline constexpr std::string_view kForbiddenReason = "Forbidden by IISRequestLevelModule";
        inline constexpr std::string_view kHeaderName = "X-IISRequestLevelModule";
        inline constexpr std::string_view kHeaderValue = "1";
        inline constexpr int kForbiddenStatusCode = 403;
        // HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) / E_ACCESSDENIED,
        // spelled as a plain value so core stays independent from
        // <windows.h>.
        inline constexpr std::int32_t kAccessDeniedError =
            static_cast<std::int32_t>(0x80070005U);

        // Case-insensitive suffix check on the cooked URL path, which
        // carries the path without the query string. Compares over a view
        // of the server-owned buffer: no copy is made. Defined in
        // ModuleHandler.cpp.
        //
        // SECURITY NOTE: this is demo logic, not an access-control
        // mechanism. A suffix-only match doesn't block a subtree
        // ("/forbidden/", "/forbidden/x" pass). Replace the matching
        // semantics before building any real ACL on this. The folding
        // itself is locale-independent — see AsciiToLower.
        auto EndsWithForbidden(const RawHttpRequest * rawRequest) -> bool;

    }

    // The IIS-independent request-handling logic: rejects requests whose
    // URL path ends with "/forbidden" and stamps every other response
    // with the X-IISRequestLevelModule header.
    //
    // OnBeginRequest is a template resolved against whatever context and
    // event provider it is handed, so production adapters and test mocks
    // both bind with no virtual dispatch and no allocation.
    //
    // It is defined here rather than in ModuleHandler.cpp because a
    // template must be visible where it is instantiated, and the types it
    // is instantiated with (the IIS adapters) live in src/module, which
    // core must not know about. Everything that is NOT type-dependent —
    // the matching algorithm — is in the .cpp.
    //
    // Stateless and const — a single instance is safely shared across
    // concurrent requests.
    class ModuleHandler {
    public:
        template <Context TContext, EventProvider TEventProvider>
        auto OnBeginRequest(TContext * context, TEventProvider * provider) const -> Verdict {
            if (context == nullptr) {
                return Verdict::Continue;
            }

            auto * request = context->GetRequest();
            auto * response = context->GetResponse();
            if (request == nullptr || response == nullptr) {
                return Verdict::Continue;
            }

            if (detail::EndsWithForbidden(request->GetRawHttpRequest())) {
                response->SetStatus(
                    detail::kForbiddenStatusCode,
                    detail::kForbiddenReason,
                    detail::kAccessDeniedError);
                if (provider != nullptr) {
                    provider->SetErrorStatus(detail::kAccessDeniedError);
                }
                return Verdict::Finish;
            }

            response->SetHeader(detail::kHeaderName, detail::kHeaderValue);
            return Verdict::Continue;
        }
    };

}
#endif // CORE_MODULE_HANDLER_H_
