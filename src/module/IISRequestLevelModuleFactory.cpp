#include "IISRequestLevelModuleFactory.h"

#include <new>

#include "IISRequestLevelModule.h"

namespace iis {

auto IISRequestLevelModuleFactory::GetHttpModule(
    _Outptr_ CHttpModule ** ppModule,
    _In_ IModuleAllocator * /*pAllocator*/) -> HRESULT {
    auto * pModule = new (std::nothrow) IISRequestLevelModule(&moduleHandler_);
    if (pModule == nullptr) {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    *ppModule = pModule;
    return S_OK;
}

auto IISRequestLevelModuleFactory::Terminate() -> VOID {
    delete this;
}

}
