#include "http_module_factory.h"

#include <new>

#include "http_module.h"

namespace iis {

auto HttpModuleFactory::GetHttpModule(
    _Outptr_ CHttpModule ** ppModule,
    _In_ IModuleAllocator * /*pAllocator*/) -> HRESULT {
    auto * pModule = new (std::nothrow) HttpModule();
    if (pModule == nullptr) {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    *ppModule = pModule;
    return S_OK;
}

auto HttpModuleFactory::Terminate() -> VOID {
    delete this;
}

}
