#ifndef IIS_CONTEXT_H_
#define IIS_CONTEXT_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/IContext.h>

#include "IISRequest.h"
#include "IISResponse.h"

namespace iis {
    // Adapter translating a live IHttpContext into core::IContext for
    // the duration of one notification. Both member adapters tolerate a
    // null IIS pointer at construction; GetRequest/GetResponse only
    // hand them out when it is non-null.
    class IISContext final : public core::IContext {
    public:
        explicit IISContext(_In_ IHttpContext * pHttpContext)
            : pRequest_{pHttpContext->GetRequest()},
              request_{pRequest_},
              pResponse_{pHttpContext->GetResponse()},
              response_{pResponse_} {}

        auto GetRequest() -> core::IRequest * override;
        auto GetResponse() -> core::IResponse * override;

    private:
        IHttpRequest * pRequest_;
        IISRequest request_;
        IHttpResponse * pResponse_;
        IISResponse response_;
    };
}
#endif // IIS_CONTEXT_H_
