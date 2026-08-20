#ifndef IIS_REQUEST_LEVEL_MODULE_H_
#define IIS_REQUEST_LEVEL_MODULE_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

namespace iis {
    // Request-level HTTP module: the only notification it handles is
    // RQ_BEGIN_REQUEST (see register_module.cpp).
    class IISRequestLevelModule : public CHttpModule {
    public:
        // RQ_BEGIN_REQUEST: rejects requests whose URL path ends with
        // "/forbidden" and stamps every other response with the
        // X-IISRequestLevelModule header.
        auto OnBeginRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // Pure virtual in CHttpModule: IIS releases module instances
        // through Dispose(), so it must be overridden.
        auto Dispose() -> VOID override;
    };
}
#endif // IIS_REQUEST_LEVEL_MODULE_H_
