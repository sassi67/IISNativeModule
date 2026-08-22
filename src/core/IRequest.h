#ifndef CORE_I_REQUEST_H_
#define CORE_I_REQUEST_H_

#include <string>

namespace core {
    // IIS-independent mapping of the parts of HTTP_REQUEST the module
    // logic reads: the cooked URL's absolute path (no query string).
    struct RawHttpRequest {
        std::wstring absPath;
    };

    // IIS-independent mapping of IHttpRequest.
    class IRequest {
    public:
        virtual ~IRequest() = default;

        // May return nullptr, mirroring an IIS request without a raw
        // request or cooked URL path.
        virtual auto GetRawHttpRequest() const -> const RawHttpRequest * = 0;
    };
}
#endif // CORE_I_REQUEST_H_
