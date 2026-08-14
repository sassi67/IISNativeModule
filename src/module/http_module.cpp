#include "http_module.h"

namespace iis {

auto HttpModule::OnBeginRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostBeginRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnAuthenticateRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IAuthenticationProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostAuthenticateRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnAuthorizeRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostAuthorizeRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnResolveRequestCache(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostResolveRequestCache(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnMapRequestHandler(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IMapHandlerProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostMapRequestHandler(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnAcquireRequestState(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostAcquireRequestState(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPreExecuteRequestHandler(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostPreExecuteRequestHandler(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnExecuteRequestHandler(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostExecuteRequestHandler(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnReleaseRequestState(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostReleaseRequestState(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnUpdateRequestCache(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostUpdateRequestCache(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnLogRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostLogRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnEndRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnPostEndRequest(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IHttpEventProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnSendResponse(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ ISendResponseProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnMapPath(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IMapPathProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnReadEntity(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ IReadEntityProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnCustomRequestNotification(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ ICustomNotificationProvider * /*pProvider*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::OnAsyncCompletion(
    _In_ IHttpContext * /*pHttpContext*/,
    _In_ DWORD /*dwNotification*/,
    _In_ BOOL /*fPostNotification*/,
    _In_ IHttpEventProvider * /*pProvider*/,
    _In_ IHttpCompletionInfo * /*pCompletionInfo*/) -> REQUEST_NOTIFICATION_STATUS {
    return RQ_NOTIFICATION_CONTINUE;
}

auto HttpModule::Dispose() -> VOID {
    // Matches the base-class behavior: IIS releases module instances
    // through Dispose(), so an empty body here would leak them.
    delete this;
}

}
