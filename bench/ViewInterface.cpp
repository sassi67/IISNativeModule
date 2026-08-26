#include "Variants.h"

// Defined out-of-line on purpose: core::ModuleHandler lives in a separate
// static library and therefore cannot be inlined into its caller either.
// Keeping variant C out-of-line too means A - C measures the copies and
// the folding function, not a difference in inlining opportunity.
namespace bench::viewiface {

auto OnBeginRequest(IContext * context, IEventProvider * provider) -> core::Verdict {
    if (context == nullptr) {
        return core::Verdict::Continue;
    }

    IRequest * request = context->GetRequest();
    IResponse * response = context->GetResponse();
    if (request == nullptr || response == nullptr) {
        return core::Verdict::Continue;
    }

    if (request->HasPath() && tmpl::EndsWithForbidden<false>(request->Path())) {
        response->SetStatus(kForbiddenStatusCode, kForbiddenReason, kAccessDeniedError);
        if (provider != nullptr) {
            provider->SetErrorStatus(kAccessDeniedError);
        }
        return core::Verdict::Finish;
    }

    response->SetHeader(kHeaderName, kHeaderValue);
    return core::Verdict::Continue;
}

}
