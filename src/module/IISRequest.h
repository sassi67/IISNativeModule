#ifndef IIS_REQUEST_H_
#define IIS_REQUEST_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

#include <core/Request.h>

namespace iis {
    // Adapter satisfying core::Request over a live IHttpRequest, for the
    // duration of one notification. Tolerates a null IIS pointer at
    // construction (GetRawHttpRequest then reports no raw request).
    //
    // The cooked URL path is VIEWED, never copied: IIS owns that buffer
    // and keeps it alive for the whole notification, which outlives this
    // stack-allocated adapter.
    class IISRequest final {
    public:
        explicit IISRequest(_In_opt_ IHttpRequest * pRequest);

        auto GetRawHttpRequest() const -> const core::RawHttpRequest *;

    private:
        core::RawHttpRequest rawRequest_;
        bool hasRawRequest_ = false;
    };

    // Fails here, with a readable message, if the class ever stops
    // matching the contract the handler template expects.
    static_assert(core::Request<IISRequest>);
}
#endif // IIS_REQUEST_H_
