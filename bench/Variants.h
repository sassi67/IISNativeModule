#ifndef BENCH_VARIANTS_H_
#define BENCH_VARIANTS_H_

#include <cstdint>
#include <cwctype>
#include <string>
#include <string_view>
#include <wchar.h>

#include <core/ModuleHandler.h>

#include "Harness.h"

// Five variants of the same request logic, differing only in dispatch
// mechanism and in whether the adapters copy the data they pass across
// the boundary. Every variant funnels identical values into bench::Sink,
// so the deltas isolate exactly one factor at a time:
//
//   A - interface + copying adapters + towlower   (production today)
//   B - template  + copying adapters + towlower   (A - B = virtual dispatch cost)
//   C - interface + view adapters    + ASCII fold (A - C = the proposed fix's win)
//   D - template  + view adapters    + ASCII fold (C - D = what templates add after the fix)
//   E - PR #8 replica: raw _wcsnicmp, no abstraction, no copies (the floor)
namespace bench {

    inline constexpr std::wstring_view kForbiddenSuffix = L"/forbidden";
    inline constexpr std::string_view kForbiddenReason = "Forbidden by IISRequestLevelModule";
    inline constexpr std::string_view kHeaderName = "X-IISRequestLevelModule";
    inline constexpr std::string_view kHeaderValue = "1";
    inline constexpr int kForbiddenStatusCode = 403;
    inline constexpr std::int32_t kAccessDeniedError = static_cast<std::int32_t>(0x80070005U);

    // Locale-independent ASCII folding, as recommended in the review.
    constexpr auto AsciiToLower(wchar_t c) -> wchar_t {
        return (c >= L'A' && c <= L'Z')
            ? static_cast<wchar_t>(c - L'A' + L'a')
            : c;
    }

    // ---------------------------------------------------------------
    // A - interface + copying adapters (mirrors src/module exactly)
    // ---------------------------------------------------------------
    namespace copying {

        class Request final : public core::IRequest {
        public:
            explicit Request(const SimulatedRawRequest * raw) {
                if (raw == nullptr || raw->CookedUrl.pAbsPath == nullptr) {
                    return;
                }
                // The production copy: IISRequest.cpp assigns into a std::wstring.
                rawRequest_.absPath.assign(
                    raw->CookedUrl.pAbsPath,
                    raw->CookedUrl.AbsPathLength / sizeof(wchar_t));
                has_ = true;
            }

            auto GetRawHttpRequest() const -> const core::RawHttpRequest * override {
                return has_ ? &rawRequest_ : nullptr;
            }

        private:
            core::RawHttpRequest rawRequest_;
            bool has_ = false;
        };

        class Response final : public core::IResponse {
        public:
            explicit Response(Sink * sink) : sink_{sink} {}

            auto SetStatus(int statusCode, std::string_view reason, std::int32_t errorStatus)
                -> void override {
                // The production copies: IISResponse.cpp materializes std::strings
                // to obtain null-terminated pointers for IIS.
                const std::string reasonString{reason};
                sink_->Consume(statusCode);
                sink_->Consume(reasonString.c_str(), reasonString.length());
                sink_->Consume(static_cast<int>(errorStatus));
            }

            auto SetHeader(std::string_view name, std::string_view value) -> void override {
                const std::string nameString{name};
                const std::string valueString{value};
                sink_->Consume(nameString.c_str(), nameString.length());
                sink_->Consume(valueString.c_str(), valueString.length());
            }

        private:
            Sink * sink_;
        };

        class Context final : public core::IContext {
        public:
            Context(const SimulatedRawRequest * raw, Sink * sink)
                : request_{raw}, response_{sink} {}

            auto GetRequest() -> core::IRequest * override { return &request_; }
            auto GetResponse() -> core::IResponse * override { return &response_; }

        private:
            Request request_;
            Response response_;
        };

        class EventProvider final : public core::IEventProvider {
        public:
            explicit EventProvider(Sink * sink) : sink_{sink} {}

            auto SetErrorStatus(std::int32_t errorStatus) -> void override {
                sink_->Consume(static_cast<int>(errorStatus));
            }

        private:
            Sink * sink_;
        };

    }

    // A: construct the adapters (as production does per request) and call
    // the real core::ModuleHandler through the interface pointers.
    inline auto RunInterfaceCopying(
        core::ModuleHandler & handler,
        const SimulatedRawRequest * raw,
        Sink * sink) -> core::Verdict {
        copying::Context context{raw, sink};
        copying::EventProvider provider{sink};
        return handler.OnBeginRequest(&context, &provider);
    }

    // ---------------------------------------------------------------
    // B - template + copying adapters (same copies, no virtual calls)
    // ---------------------------------------------------------------
    namespace tmpl {

        class CopyRequest {
        public:
            explicit CopyRequest(const SimulatedRawRequest * raw) {
                if (raw == nullptr || raw->CookedUrl.pAbsPath == nullptr) {
                    return;
                }
                absPath_.assign(
                    raw->CookedUrl.pAbsPath,
                    raw->CookedUrl.AbsPathLength / sizeof(wchar_t));
                has_ = true;
            }

            auto HasPath() const -> bool { return has_; }
            auto Path() const -> std::wstring_view { return absPath_; }

        private:
            std::wstring absPath_;
            bool has_ = false;
        };

        class CopyResponse {
        public:
            explicit CopyResponse(Sink * sink) : sink_{sink} {}

            auto SetStatus(int statusCode, std::string_view reason, std::int32_t errorStatus) -> void {
                const std::string reasonString{reason};
                sink_->Consume(statusCode);
                sink_->Consume(reasonString.c_str(), reasonString.length());
                sink_->Consume(static_cast<int>(errorStatus));
            }

            auto SetHeader(std::string_view name, std::string_view value) -> void {
                const std::string nameString{name};
                const std::string valueString{value};
                sink_->Consume(nameString.c_str(), nameString.length());
                sink_->Consume(valueString.c_str(), valueString.length());
            }

        private:
            Sink * sink_;
        };

        // Zero-copy flavours used by variants C/D: a view straight over the
        // IIS-owned buffer, and pass-through of the (literal-backed,
        // null-terminated) constants.
        class ViewRequest {
        public:
            explicit ViewRequest(const SimulatedRawRequest * raw) {
                if (raw == nullptr || raw->CookedUrl.pAbsPath == nullptr) {
                    return;
                }
                absPath_ = std::wstring_view(
                    raw->CookedUrl.pAbsPath,
                    raw->CookedUrl.AbsPathLength / sizeof(wchar_t));
                has_ = true;
            }

            auto HasPath() const -> bool { return has_; }
            auto Path() const -> std::wstring_view { return absPath_; }

        private:
            std::wstring_view absPath_;
            bool has_ = false;
        };

        class ViewResponse {
        public:
            explicit ViewResponse(Sink * sink) : sink_{sink} {}

            auto SetStatus(int statusCode, std::string_view reason, std::int32_t errorStatus) -> void {
                sink_->Consume(statusCode);
                sink_->Consume(reason.data(), reason.length());
                sink_->Consume(static_cast<int>(errorStatus));
            }

            auto SetHeader(std::string_view name, std::string_view value) -> void {
                sink_->Consume(name.data(), name.length());
                sink_->Consume(value.data(), value.length());
            }

        private:
            Sink * sink_;
        };

        class EventProvider {
        public:
            explicit EventProvider(Sink * sink) : sink_{sink} {}

            auto SetErrorStatus(std::int32_t errorStatus) -> void {
                sink_->Consume(static_cast<int>(errorStatus));
            }

        private:
            Sink * sink_;
        };

        template <typename TRequest, typename TResponse>
        class Context {
        public:
            Context(const SimulatedRawRequest * raw, Sink * sink)
                : request_{raw}, response_{sink} {}

            auto GetRequest() -> TRequest & { return request_; }
            auto GetResponse() -> TResponse & { return response_; }

        private:
            TRequest request_;
            TResponse response_;
        };

        // UseLocaleFold selects towlower (matching today's core logic) vs
        // the recommended locale-independent ASCII folding.
        template <bool UseLocaleFold>
        auto EndsWithForbidden(std::wstring_view path) -> bool {
            if (path.length() < kForbiddenSuffix.length()) {
                return false;
            }
            const std::wstring_view tail =
                path.substr(path.length() - kForbiddenSuffix.length());
            for (size_t i = 0; i < kForbiddenSuffix.length(); ++i) {
                if constexpr (UseLocaleFold) {
                    if (std::towlower(tail[i]) != std::towlower(kForbiddenSuffix[i])) {
                        return false;
                    }
                } else {
                    if (AsciiToLower(tail[i]) != AsciiToLower(kForbiddenSuffix[i])) {
                        return false;
                    }
                }
            }
            return true;
        }

        // The template equivalent of core::ModuleHandler::OnBeginRequest:
        // identical logic, resolved at compile time.
        template <bool UseLocaleFold, typename TContext, typename TEventProvider>
        auto OnBeginRequest(TContext & context, TEventProvider & provider) -> core::Verdict {
            auto & request = context.GetRequest();
            auto & response = context.GetResponse();

            if (request.HasPath() && EndsWithForbidden<UseLocaleFold>(request.Path())) {
                response.SetStatus(kForbiddenStatusCode, kForbiddenReason, kAccessDeniedError);
                provider.SetErrorStatus(kAccessDeniedError);
                return core::Verdict::Finish;
            }

            response.SetHeader(kHeaderName, kHeaderValue);
            return core::Verdict::Continue;
        }

    }

    inline auto RunTemplateCopying(const SimulatedRawRequest * raw, Sink * sink) -> core::Verdict {
        tmpl::Context<tmpl::CopyRequest, tmpl::CopyResponse> context{raw, sink};
        tmpl::EventProvider provider{sink};
        return tmpl::OnBeginRequest<true>(context, provider);
    }

    inline auto RunTemplateView(const SimulatedRawRequest * raw, Sink * sink) -> core::Verdict {
        tmpl::Context<tmpl::ViewRequest, tmpl::ViewResponse> context{raw, sink};
        tmpl::EventProvider provider{sink};
        return tmpl::OnBeginRequest<false>(context, provider);
    }

    // ---------------------------------------------------------------
    // C - interface + view adapters + ASCII fold (the recommended fix,
    //     keeping runtime polymorphism). Uses a bench-local interface set
    //     because the fix changes the interface shape (view, not copy).
    // ---------------------------------------------------------------
    namespace viewiface {

        class IRequest {
        public:
            virtual ~IRequest() = default;
            virtual auto HasPath() const -> bool = 0;
            virtual auto Path() const -> std::wstring_view = 0;
        };

        class IResponse {
        public:
            virtual ~IResponse() = default;
            virtual auto SetStatus(int statusCode, std::string_view reason, std::int32_t errorStatus) -> void = 0;
            virtual auto SetHeader(std::string_view name, std::string_view value) -> void = 0;
        };

        class IContext {
        public:
            virtual ~IContext() = default;
            virtual auto GetRequest() -> IRequest * = 0;
            virtual auto GetResponse() -> IResponse * = 0;
        };

        class IEventProvider {
        public:
            virtual ~IEventProvider() = default;
            virtual auto SetErrorStatus(std::int32_t errorStatus) -> void = 0;
        };

        class Request final : public IRequest {
        public:
            explicit Request(const SimulatedRawRequest * raw) {
                if (raw == nullptr || raw->CookedUrl.pAbsPath == nullptr) {
                    return;
                }
                absPath_ = std::wstring_view(
                    raw->CookedUrl.pAbsPath,
                    raw->CookedUrl.AbsPathLength / sizeof(wchar_t));
                has_ = true;
            }

            auto HasPath() const -> bool override { return has_; }
            auto Path() const -> std::wstring_view override { return absPath_; }

        private:
            std::wstring_view absPath_;
            bool has_ = false;
        };

        class Response final : public IResponse {
        public:
            explicit Response(Sink * sink) : sink_{sink} {}

            auto SetStatus(int statusCode, std::string_view reason, std::int32_t errorStatus)
                -> void override {
                sink_->Consume(statusCode);
                sink_->Consume(reason.data(), reason.length());
                sink_->Consume(static_cast<int>(errorStatus));
            }

            auto SetHeader(std::string_view name, std::string_view value) -> void override {
                sink_->Consume(name.data(), name.length());
                sink_->Consume(value.data(), value.length());
            }

        private:
            Sink * sink_;
        };

        class Context final : public IContext {
        public:
            Context(const SimulatedRawRequest * raw, Sink * sink)
                : request_{raw}, response_{sink} {}

            auto GetRequest() -> IRequest * override { return &request_; }
            auto GetResponse() -> IResponse * override { return &response_; }

        private:
            Request request_;
            Response response_;
        };

        class EventProvider final : public IEventProvider {
        public:
            explicit EventProvider(Sink * sink) : sink_{sink} {}

            auto SetErrorStatus(std::int32_t errorStatus) -> void override {
                sink_->Consume(static_cast<int>(errorStatus));
            }

        private:
            Sink * sink_;
        };

        // Deliberately compiled here rather than in the core library so the
        // only difference from variant D is the dispatch mechanism.
        auto OnBeginRequest(IContext * context, IEventProvider * provider) -> core::Verdict;

    }

    inline auto RunInterfaceView(const SimulatedRawRequest * raw, Sink * sink) -> core::Verdict {
        viewiface::Context context{raw, sink};
        viewiface::EventProvider provider{sink};
        return viewiface::OnBeginRequest(&context, &provider);
    }

    // ---------------------------------------------------------------
    // E - PR #8 replica: read the raw buffer in place, _wcsnicmp, no
    //     abstraction and no allocation.
    // ---------------------------------------------------------------
    inline auto RunPr8Baseline(const SimulatedRawRequest * raw, Sink * sink) -> core::Verdict {
        if (raw == nullptr || raw->CookedUrl.pAbsPath == nullptr) {
            return core::Verdict::Continue;
        }

        const size_t pathLength = raw->CookedUrl.AbsPathLength / sizeof(wchar_t);
        const size_t suffixLength = kForbiddenSuffix.length();
        const bool forbidden =
            pathLength >= suffixLength &&
            _wcsnicmp(
                raw->CookedUrl.pAbsPath + (pathLength - suffixLength),
                kForbiddenSuffix.data(),
                suffixLength) == 0;

        if (forbidden) {
            sink->Consume(kForbiddenStatusCode);
            sink->Consume(kForbiddenReason.data(), kForbiddenReason.length());
            sink->Consume(static_cast<int>(kAccessDeniedError));
            sink->Consume(static_cast<int>(kAccessDeniedError));
            return core::Verdict::Finish;
        }

        sink->Consume(kHeaderName.data(), kHeaderName.length());
        sink->Consume(kHeaderValue.data(), kHeaderValue.length());
        return core::Verdict::Continue;
    }

}
#endif // BENCH_VARIANTS_H_
