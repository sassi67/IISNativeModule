#include "IISContext.h"

namespace iis {

auto IISContext::GetRequest() -> core::IRequest * {
    return pRequest_ != nullptr ? &request_ : nullptr;
}

auto IISContext::GetResponse() -> core::IResponse * {
    return pResponse_ != nullptr ? &response_ : nullptr;
}

}
