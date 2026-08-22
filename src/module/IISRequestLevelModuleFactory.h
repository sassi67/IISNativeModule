#ifndef IIS_REQUEST_LEVEL_MODULE_FACTORY_H_
#define IIS_REQUEST_LEVEL_MODULE_FACTORY_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/ModuleHandler.h>

namespace iis {
    // Creates an IISRequestLevelModule instance for each request IIS
    // dispatches to this module, injecting the shared ModuleHandler
    // that carries the actual request logic. The factory outlives every
    // module instance (IIS terminates factories at shutdown), so
    // handing out a pointer to the member is safe; ModuleHandler is
    // stateless, so sharing it across concurrent requests is too.
    class IISRequestLevelModuleFactory : public IHttpModuleFactory {
    public:
        auto GetHttpModule(
            _Outptr_ CHttpModule ** ppModule,
            _In_ IModuleAllocator * pAllocator) -> HRESULT override;

        auto Terminate() -> VOID override;

    private:
        core::ModuleHandler moduleHandler_;
    };
}
#endif // IIS_REQUEST_LEVEL_MODULE_FACTORY_H_
