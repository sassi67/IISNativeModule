#include "IISEventProvider.h"

namespace iis {

auto IISEventProvider::SetErrorStatus(std::int32_t errorStatus) -> void {
    pProvider_->SetErrorStatus(static_cast<HRESULT>(errorStatus));
}

}
