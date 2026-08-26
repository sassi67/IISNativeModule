#ifndef IIS_CONTEXT_H_
#define IIS_CONTEXT_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/Context.h>

#include "IISRequest.h"
#include "IISResponse.h"

namespace iis {
    // Adapter satisfying core::Context over a live IHttpContext, for the
    // duration of one notification. Both member adapters tolerate a null
    // IIS pointer at construction; GetRequest/GetResponse only hand them
    // out when it is non-null.
    class IISContext final {
    public:
        explicit IISContext(_In_ IHttpContext * pHttpContext);

        auto GetRequest() -> IISRequest *;
        auto GetResponse() -> IISResponse *;

    private:
        IHttpRequest * pRequest_;
        IISRequest request_;
        IHttpResponse * pResponse_;
        IISResponse response_;
    };

    // Fails here, with a readable message, if the class ever stops
    // matching the contract the handler template expects. Transitively
    // checks IISRequest and IISResponse too.
    static_assert(core::Context<IISContext>);
}
#endif // IIS_CONTEXT_H_
