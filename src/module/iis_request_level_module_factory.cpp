#include "iis_request_level_module_factory.h"

#include <new>

#include "iis_request_level_module.h"

namespace iis {

auto IISRequestLevelModuleFactory::GetHttpModule(
    _Outptr_ CHttpModule ** ppModule,
    _In_ IModuleAllocator * /*pAllocator*/) -> HRESULT {
    auto * pModule = new (std::nothrow) IISRequestLevelModule();
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
