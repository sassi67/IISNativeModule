#ifndef IIS_REQUEST_H_
#define IIS_REQUEST_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/IRequest.h>

namespace iis {
    // Adapter translating a live IHttpRequest into core::IRequest for
    // the duration of one notification. Tolerates a null IIS pointer at
    // construction (GetRawHttpRequest then reports no raw request).
    class IISRequest final : public core::IRequest {
    public:
        explicit IISRequest(_In_opt_ IHttpRequest * pRequest);

        auto GetRawHttpRequest() const -> const core::RawHttpRequest * override;

    private:
        core::RawHttpRequest rawRequest_;
        bool hasRawRequest_ = false;
    };
}
#endif // IIS_REQUEST_H_
