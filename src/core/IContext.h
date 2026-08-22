#ifndef CORE_I_CONTEXT_H_
#define CORE_I_CONTEXT_H_

#include "IRequest.h"
#include "IResponse.h"

namespace core {
    // IIS-independent mapping of IHttpContext.
    class IContext {
    public:
        virtual ~IContext() = default;

        // Either may return nullptr, mirroring an IIS context whose
        // request or response is unavailable.
        virtual auto GetRequest() -> IRequest * = 0;
        virtual auto GetResponse() -> IResponse * = 0;
    };
}
#endif // CORE_I_CONTEXT_H_
