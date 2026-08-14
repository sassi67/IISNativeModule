#ifndef HTTP_MODULE_H_
#define HTTP_MODULE_H_

#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>

namespace iis {
    // Request-level HTTP module skeleton: overrides every overridable
    // method of CHttpModule, with empty bodies to be filled in later.
    class HttpModule : public CHttpModule {
    public:
        // RQ_BEGIN_REQUEST
        auto OnBeginRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostBeginRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_AUTHENTICATE_REQUEST
        auto OnAuthenticateRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IAuthenticationProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostAuthenticateRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_AUTHORIZE_REQUEST
        auto OnAuthorizeRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostAuthorizeRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_RESOLVE_REQUEST_CACHE
        auto OnResolveRequestCache(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostResolveRequestCache(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_MAP_REQUEST_HANDLER
        auto OnMapRequestHandler(
            _In_ IHttpContext * pHttpContext,
            _In_ IMapHandlerProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostMapRequestHandler(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_ACQUIRE_REQUEST_STATE
        auto OnAcquireRequestState(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostAcquireRequestState(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_PRE_EXECUTE_REQUEST_HANDLER
        auto OnPreExecuteRequestHandler(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostPreExecuteRequestHandler(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_EXECUTE_REQUEST_HANDLER
        auto OnExecuteRequestHandler(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostExecuteRequestHandler(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_RELEASE_REQUEST_STATE
        auto OnReleaseRequestState(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostReleaseRequestState(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_UPDATE_REQUEST_CACHE
        auto OnUpdateRequestCache(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostUpdateRequestCache(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_LOG_REQUEST
        auto OnLogRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostLogRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_END_REQUEST
        auto OnEndRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        auto OnPostEndRequest(
            _In_ IHttpContext * pHttpContext,
            _In_ IHttpEventProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_SEND_RESPONSE
        auto OnSendResponse(
            _In_ IHttpContext * pHttpContext,
            _In_ ISendResponseProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_MAP_PATH
        auto OnMapPath(
            _In_ IHttpContext * pHttpContext,
            _In_ IMapPathProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_READ_ENTITY
        auto OnReadEntity(
            _In_ IHttpContext * pHttpContext,
            _In_ IReadEntityProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // RQ_CUSTOM_NOTIFICATION
        auto OnCustomRequestNotification(
            _In_ IHttpContext * pHttpContext,
            _In_ ICustomNotificationProvider * pProvider) -> REQUEST_NOTIFICATION_STATUS override;

        // Completion
        auto OnAsyncCompletion(
            _In_ IHttpContext * pHttpContext,
            _In_ DWORD dwNotification,
            _In_ BOOL fPostNotification,
            _In_ IHttpEventProvider * pProvider,
            _In_ IHttpCompletionInfo * pCompletionInfo) -> REQUEST_NOTIFICATION_STATUS override;

        auto Dispose() -> VOID override;
    };
}
#endif // HTTP_MODULE_H_
