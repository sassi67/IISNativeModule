#include "IISRequest.h"

namespace iis {

IISRequest::IISRequest(_In_opt_ IHttpRequest * pRequest) {
    if (pRequest == nullptr) {
        return;
    }
    const HTTP_REQUEST * pRawRequest = pRequest->GetRawHttpRequest();
    if (pRawRequest == nullptr || pRawRequest->CookedUrl.pAbsPath == nullptr) {
        return;
    }
    // The cooked URL length is in bytes. A view, not a copy: the buffer
    // belongs to IIS and outlives this adapter.
    rawRequest_.absPath = std::wstring_view(
        pRawRequest->CookedUrl.pAbsPath,
        pRawRequest->CookedUrl.AbsPathLength / sizeof(WCHAR));
    hasRawRequest_ = true;
}

auto IISRequest::GetRawHttpRequest() const -> const core::RawHttpRequest * {
    return hasRawRequest_ ? &rawRequest_ : nullptr;
}

}
