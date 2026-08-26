#include "IISContext.h"

namespace iis {

// Both adapters tolerate a null IIS pointer at construction; the getters
// below only hand them out when it is non-null.
IISContext::IISContext(_In_ IHttpContext * pHttpContext)
    : pRequest_{pHttpContext->GetRequest()},
      request_{pRequest_},
      pResponse_{pHttpContext->GetResponse()},
      response_{pResponse_} {}

auto IISContext::GetRequest() -> IISRequest * {
    return pRequest_ != nullptr ? &request_ : nullptr;
}

auto IISContext::GetResponse() -> IISResponse * {
    return pResponse_ != nullptr ? &response_ : nullptr;
}

}
