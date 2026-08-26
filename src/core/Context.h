#ifndef CORE_CONTEXT_H_
#define CORE_CONTEXT_H_

#include <type_traits>
#include <utility>

#include "Request.h"
#include "Response.h"

namespace core {

    namespace detail {
        // GetRequest()/GetResponse() hand back pointers; this strips the
        // pointer and cv-qualifiers so the concepts below can be applied
        // to what they point at.
        template <typename T>
        using Pointee = std::remove_cvref_t<std::remove_pointer_t<T>>;
    }

    // Compile-time counterpart of IHttpContext: exposes a request and a
    // response that themselves satisfy the Request/Response concepts.
    //
    // Both getters return nullable pointers, mirroring an IIS context
    // whose request or response is unavailable.
    template <typename T>
    concept Context =
        requires(T & context) {
            context.GetRequest();
            context.GetResponse();
        } &&
        Request<detail::Pointee<decltype(std::declval<T &>().GetRequest())>> &&
        Response<detail::Pointee<decltype(std::declval<T &>().GetResponse())>>;

}
#endif // CORE_CONTEXT_H_
