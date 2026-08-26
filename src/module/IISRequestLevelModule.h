#ifndef IIS_REQUEST_LEVEL_MODULE_H_
#define IIS_REQUEST_LEVEL_MODULE_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/ModuleHandler.h>

namespace iis {
    // Request-level HTTP module: the only notification it handles is
    // RQ_BEGIN_REQUEST (see register_module.cpp). The actual request
    // logic lives in the injected core::ModuleHandler, keeping this class
    // a thin IIS adapter; handler is non-owning and must outlive this
    // instance (the factory owns it).
    //
    // The handler is held by concrete pointer rather than through an
    // abstract base: its OnBeginRequest is a template, so substituting a
    // different implementation happens at compile time (which is what
    // lets the whole call chain inline and stay allocation-free) instead
    // of through a vtable.
    class IISRequestLevelModule : public CHttpModule {
    public:
        explicit IISRequestLevelModule(const core::ModuleHandler * handler) : handler_{handler} {}

        // RQ_BEGIN_REQUEST: wraps the IIS context/provider in the core
        // adapters, delegates to handler_, and maps the returned Verdict
        // back to a REQUEST_NOTIFICATION_STATUS.
        auto OnBeginRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // IIS releases module instances through Dispose().
        auto Dispose() -> VOID override;

    private:
        const core::ModuleHandler * handler_;
    };
}
#endif // IIS_REQUEST_LEVEL_MODULE_H_
