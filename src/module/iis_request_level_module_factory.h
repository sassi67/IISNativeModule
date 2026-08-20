#ifndef IIS_REQUEST_LEVEL_MODULE_FACTORY_H_
#define IIS_REQUEST_LEVEL_MODULE_FACTORY_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

namespace iis {
    // Creates an IISRequestLevelModule instance for each request IIS
    // dispatches to this module.
    class IISRequestLevelModuleFactory : public IHttpModuleFactory {
    public:
        auto GetHttpModule(
            _Outptr_ CHttpModule ** ppModule,
            _In_ IModuleAllocator * pAllocator) -> HRESULT override;

        auto Terminate() -> VOID override;
    };
}
#endif // IIS_REQUEST_LEVEL_MODULE_FACTORY_H_
