#ifndef IIS_REQUEST_LEVEL_MODULE_H_
#define IIS_REQUEST_LEVEL_MODULE_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/IModule.h>

namespace iis {
    // Request-level HTTP module: the only notification it handles is
    // RQ_BEGIN_REQUEST (see register_module.cpp). The actual request
    // logic lives in the injected core::IModule, keeping this class a
    // thin IIS adapter; module is non-owning and must outlive this
    // instance (the factory owns the handler).
    class IISRequestLevelModule : public CHttpModule {
    public:
        explicit IISRequestLevelModule(core::IModule * module) : module_{module} {}

        // RQ_BEGIN_REQUEST: translates the IIS context/provider into
        // their core counterparts, delegates to module_, and maps the
        // returned Verdict back to a REQUEST_NOTIFICATION_STATUS.
        auto OnBeginRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // IIS releases module instances through Dispose().
        auto Dispose() -> VOID override;

    private:
        core::IModule * module_;
    };
}
#endif // IIS_REQUEST_LEVEL_MODULE_H_
