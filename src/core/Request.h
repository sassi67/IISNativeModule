#ifndef CORE_REQUEST_H_
#define CORE_REQUEST_H_

#include <concepts>
#include <string_view>

namespace core {
    // IIS-independent mapping of the parts of HTTP_REQUEST the module
    // logic reads: the cooked URL's absolute path (no query string).
    //
    // absPath is a NON-OWNING view over a buffer the server owns. It is
    // valid only for the notification the request object was built for,
    // which is exactly that object's lifetime — never store it beyond
    // that. Holding a view rather than a std::wstring is what keeps the
    // hot path allocation-free.
    struct RawHttpRequest {
        std::wstring_view absPath;
    };

    // Compile-time counterpart of IHttpRequest: any type exposing
    // GetRawHttpRequest() satisfies it, so production adapters and test
    // mocks are interchangeable without a vtable.
    //
    // The pointer is nullable, mirroring an IIS request that has no raw
    // request or no cooked URL path.
    template <typename T>
    concept Request = requires(const T & request) {
        { request.GetRawHttpRequest() } -> std::same_as<const RawHttpRequest *>;
    };
}
#endif // CORE_REQUEST_H_
