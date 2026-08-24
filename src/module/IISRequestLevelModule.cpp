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

    // IIS notification methods must not throw: an escaping C++ exception
    // unwinds into the native pipeline and crashes the worker process
    // (the adapters allocate, so std::bad_alloc is possible under memory
    // pressure). Failing open (Continue) is a deliberate tradeoff for
    // this demo rule: a dropped forbidden-check beats taking down the
    // whole app pool. catch(...) under /EHsc catches C++ exceptions
    // only, not SEH.
    try {
        IISContext context{pHttpContext};
        IISEventProvider provider{pProvider};

        const core::Verdict verdict = module_->OnBeginRequest(
            &context,
            pProvider != nullptr ? &provider : nullptr);

        return verdict == core::Verdict::Finish
            ? RQ_NOTIFICATION_FINISH_REQUEST
            : RQ_NOTIFICATION_CONTINUE;
    } catch (...) {
        return RQ_NOTIFICATION_CONTINUE;
    }
}

auto IISRequestLevelModule::Dispose() -> VOID {
    delete this;
}

}
