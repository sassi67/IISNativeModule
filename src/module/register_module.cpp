#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <new>

#include "IISRequestLevelModuleFactory.h"

// Entry point IIS calls when it loads the module DLL; exported via
// iis_native_module.def.
HRESULT WINAPI RegisterModule(
    DWORD /*dwServerVersion*/,
    IHttpModuleRegistrationInfo * pModuleInfo,
    IHttpServer * /*pGlobalInfo*/) {
    auto * pFactory = new (std::nothrow) iis::IISRequestLevelModuleFactory();
    if (pFactory == nullptr) {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    // IISRequestLevelModule only implements OnBeginRequest, so that is
    // the single request notification we subscribe to. IIS takes
    // ownership of the factory only on success.
    const HRESULT hr = pModuleInfo->SetRequestNotifications(pFactory, RQ_BEGIN_REQUEST, 0);
    if (FAILED(hr)) {
        delete pFactory;
    }
    return hr;
}
