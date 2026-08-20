#include "iis_request_level_module.h"

#include <string.h>
#include <wchar.h>

namespace iis {

namespace {

    constexpr PCWSTR kForbiddenSuffix = L"/forbidden";
    constexpr PCSTR kForbiddenReason = "Forbidden by IISRequestLevelModule";
    constexpr PCSTR kHeaderName = "X-IISRequestLevelModule";
    constexpr PCSTR kHeaderValue = "1";

    // The cooked URL carries the path without the query string, which is
    // what the "/forbidden" check is about. Its length is in bytes.
    auto EndsWithForbidden(_In_ const HTTP_REQUEST * pRawRequest) -> bool {
        if (pRawRequest == nullptr || pRawRequest->CookedUrl.pAbsPath == nullptr) {
            return false;
        }
        const size_t pathLength = pRawRequest->CookedUrl.AbsPathLength / sizeof(WCHAR);
        const size_t suffixLength = wcslen(kForbiddenSuffix);
        if (pathLength < suffixLength) {
            return false;
        }
        return _wcsnicmp(
            pRawRequest->CookedUrl.pAbsPath + (pathLength - suffixLength),
            kForbiddenSuffix,
            suffixLength) == 0;
    }

}

auto IISRequestLevelModule::OnBeginRequest(
    _In_ IHttpContext * pHttpContext,
    _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS {
    if (pHttpContext == nullptr) {
        return RQ_NOTIFICATION_CONTINUE;
    }

    IHttpRequest * pRequest = pHttpContext->GetRequest();
    IHttpResponse * pResponse = pHttpContext->GetResponse();
    if (pRequest == nullptr || pResponse == nullptr) {
        return RQ_NOTIFICATION_CONTINUE;
    }

    if (EndsWithForbidden(pRequest->GetRawHttpRequest())) {
        const HRESULT hrAccessDenied = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
        // fTrySkipCustomErrors: keep our own reason phrase instead of
        // letting the custom-errors module replace the response.
        pResponse->SetStatus(403, kForbiddenReason, 0, hrAccessDenied, nullptr, TRUE);
        if (pProvider != nullptr) {
            pProvider->SetErrorStatus(hrAccessDenied);
        }
        return RQ_NOTIFICATION_FINISH_REQUEST;
    }

    pResponse->SetHeader(
        kHeaderName,
        kHeaderValue,
        static_cast<USHORT>(strlen(kHeaderValue)),
        TRUE);
    return RQ_NOTIFICATION_CONTINUE;
}

auto IISRequestLevelModule::Dispose() -> VOID {
    delete this;
}

}
