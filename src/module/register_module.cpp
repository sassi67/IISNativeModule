#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <new>

#include "http_module_factory.h"

// Entry point IIS calls when it loads the module DLL; exported via
// iis_native_module.def.
HRESULT WINAPI RegisterModule(
    DWORD /*dwServerVersion*/,
    IHttpModuleRegistrationInfo * pModuleInfo,
    IHttpServer * /*pGlobalInfo*/) {
    auto * pFactory = new (std::nothrow) iis::HttpModuleFactory();
    if (pFactory == nullptr) {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    // Skeleton: no request notifications subscribed yet. Add RQ_* flags
    // here as the module methods gain real implementations.
    return pModuleInfo->SetRequestNotifications(pFactory, 0, 0);
}
