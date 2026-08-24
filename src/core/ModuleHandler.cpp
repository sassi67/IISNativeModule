#include "ModuleHandler.h"

#include <cstdint>
#include <cwctype>
#include <string_view>

namespace core {

namespace {

    constexpr std::wstring_view kForbiddenSuffix = L"/forbidden";
    constexpr std::string_view kForbiddenReason = "Forbidden by IISRequestLevelModule";
    constexpr std::string_view kHeaderName = "X-IISRequestLevelModule";
    constexpr std::string_view kHeaderValue = "1";
    constexpr int kForbiddenStatusCode = 403;
    // HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) / E_ACCESSDENIED, spelled
    // as a plain value so core stays independent from <windows.h>.
    constexpr std::int32_t kAccessDeniedError =
        static_cast<std::int32_t>(0x80070005U);

    // Case-insensitive suffix check on the cooked URL path, which
    // carries the path without the query string.
    //
    // SECURITY NOTE: this is demo logic, not an access-control
    // mechanism. A suffix-only match doesn't block a subtree
    // ("/forbidden/", "/forbidden/x" pass), and std::towlower is
    // locale-dependent (invariant ASCII folding is what a security
    // comparison would need). Replace the matching semantics before
    // building any real ACL on this.
    auto EndsWithForbidden(const RawHttpRequest * rawRequest) -> bool {
        if (rawRequest == nullptr) {
            return false;
        }
        const std::wstring_view path = rawRequest->absPath;
        if (path.length() < kForbiddenSuffix.length()) {
            return false;
        }
        const std::wstring_view tail = path.substr(path.length() - kForbiddenSuffix.length());
        for (size_t i = 0; i < kForbiddenSuffix.length(); ++i) {
            if (std::towlower(tail[i]) != std::towlower(kForbiddenSuffix[i])) {
                return false;
            }
        }
        return true;
    }

}

auto ModuleHandler::OnBeginRequest(
    IContext * context,
    IEventProvider * provider) -> Verdict {
    if (context == nullptr) {
        return Verdict::Continue;
    }

    IRequest * request = context->GetRequest();
    IResponse * response = context->GetResponse();
    if (request == nullptr || response == nullptr) {
        return Verdict::Continue;
    }

    if (EndsWithForbidden(request->GetRawHttpRequest())) {
        response->SetStatus(kForbiddenStatusCode, kForbiddenReason, kAccessDeniedError);
        if (provider != nullptr) {
            provider->SetErrorStatus(kAccessDeniedError);
        }
        return Verdict::Finish;
    }

    response->SetHeader(kHeaderName, kHeaderValue);
    return Verdict::Continue;
}

}
