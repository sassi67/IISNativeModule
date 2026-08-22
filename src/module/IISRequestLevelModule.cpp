#include "IISRequestLevelModule.h"

#include <core/Verdict.h>

#include "IISContext.h"
#include "IISEventProvider.h"

namespace iis {

auto IISRequestLevelModule::OnBeginRequest(
    _In_ IHttpContext * pHttpContext,
    _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS {
    if (module_ == nullptr || pHttpContext == nullptr) {
        return RQ_NOTIFICATION_CONTINUE;
    }

    IISContext context{pHttpContext};
    IISEventProvider provider{pProvider};

    const core::Verdict verdict = module_->OnBeginRequest(
        &context,
        pProvider != nullptr ? &provider : nullptr);

    return verdict == core::Verdict::Finish
        ? RQ_NOTIFICATION_FINISH_REQUEST
        : RQ_NOTIFICATION_CONTINUE;
}

auto IISRequestLevelModule::Dispose() -> VOID {
    delete this;
}

}
