#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <core/ModuleHandler.h>

namespace core {

namespace {

    // The mocks satisfy the core concepts by shape alone — no base class
    // and no vtable — which is exactly how the production adapters bind.
    class MockRequest {
    public:
        explicit MockRequest(std::wstring url) : url_{std::move(url)} {
            rawRequest_.absPath = url_;
        }

        // rawRequest_ views url_, so copying would leave the view pointing
        // at the moved-from string.
        MockRequest(const MockRequest &) = delete;
        auto operator=(const MockRequest &) -> MockRequest & = delete;

        auto GetRawHttpRequest() const -> const RawHttpRequest * {
            return &rawRequest_;
        }

    private:
        std::wstring url_;
        RawHttpRequest rawRequest_;
    };

    // Records every call so the tests can assert on what the handler did
    // to the response.
    class MockResponse {
    public:
        auto SetStatus(
            int statusCode,
            std::string_view reason,
            std::int32_t errorStatus) -> void {
            statusCode_ = statusCode;
            reason_ = std::string{reason};
            errorStatus_ = errorStatus;
        }

        auto SetHeader(std::string_view name, std::string_view value) -> void {
            headers_.emplace_back(std::string{name}, std::string{value});
        }

        int statusCode_ = 0;
        std::string reason_;
        std::int32_t errorStatus_ = 0;
        std::vector<std::pair<std::string, std::string>> headers_;
    };

    class MockContext {
    public:
        MockContext(MockRequest * request, MockResponse * response)
            : request_{request}, response_{response} {}

        auto GetRequest() -> MockRequest * { return request_; }
        auto GetResponse() -> MockResponse * { return response_; }

    private:
        MockRequest * request_;
        MockResponse * response_;
    };

    class MockEventProvider {
    public:
        auto SetErrorStatus(std::int32_t errorStatus) -> void {
            errorStatus_ = errorStatus;
        }

        std::int32_t errorStatus_ = 0;
    };

    static_assert(Request<MockRequest>);
    static_assert(Response<MockResponse>);
    static_assert(Context<MockContext>);
    static_assert(EventProvider<MockEventProvider>);

    // HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED), as the plain value the
    // handler reports for forbidden requests.
    constexpr std::int32_t kAccessDeniedError =
        static_cast<std::int32_t>(0x80070005U);

}

TEST(TestCore, TestModuleHandlerOnBeginRequestForbiddenUrl)
{
    MockRequest request{L"/app/forbidden"};
    MockResponse response;
    MockContext context{&request, &response};
    MockEventProvider provider;
    const ModuleHandler handler;

    const Verdict verdict = handler.OnBeginRequest(&context, &provider);

    EXPECT_EQ(Verdict::Finish, verdict);
    EXPECT_EQ(403, response.statusCode_);
    EXPECT_EQ("Forbidden by IISRequestLevelModule", response.reason_);
    EXPECT_EQ(kAccessDeniedError, response.errorStatus_);
    EXPECT_EQ(kAccessDeniedError, provider.errorStatus_);
    EXPECT_TRUE(response.headers_.empty());
}

TEST(TestCore, TestModuleHandlerOnBeginRequestGenericUrl)
{
    MockRequest request{L"/app/index.html"};
    MockResponse response;
    MockContext context{&request, &response};
    MockEventProvider provider;
    const ModuleHandler handler;

    const Verdict verdict = handler.OnBeginRequest(&context, &provider);

    EXPECT_EQ(Verdict::Continue, verdict);
    EXPECT_EQ(0, response.statusCode_);
    EXPECT_EQ(0, provider.errorStatus_);
    ASSERT_EQ(1U, response.headers_.size());
    EXPECT_EQ("X-IISRequestLevelModule", response.headers_[0].first);
    EXPECT_EQ("1", response.headers_[0].second);
}

// The suffix match folds case, and must do so identically regardless of
// the active locale — see core::AsciiToLower.
TEST(TestCore, TestModuleHandlerOnBeginRequestForbiddenUrlIsCaseInsensitive)
{
    MockRequest request{L"/app/FoRbIdDeN"};
    MockResponse response;
    MockContext context{&request, &response};
    MockEventProvider provider;
    const ModuleHandler handler;

    const Verdict verdict = handler.OnBeginRequest(&context, &provider);

    EXPECT_EQ(Verdict::Finish, verdict);
    EXPECT_EQ(403, response.statusCode_);
}

// A path merely containing "/forbidden" further up does not end with it,
// so it is not rejected (documents the suffix-only semantics called out
// in ModuleHandler.h).
TEST(TestCore, TestModuleHandlerOnBeginRequestForbiddenPrefixIsAllowed)
{
    MockRequest request{L"/forbidden/child.html"};
    MockResponse response;
    MockContext context{&request, &response};
    MockEventProvider provider;
    const ModuleHandler handler;

    const Verdict verdict = handler.OnBeginRequest(&context, &provider);

    EXPECT_EQ(Verdict::Continue, verdict);
    ASSERT_EQ(1U, response.headers_.size());
}

TEST(TestCore, TestModuleHandlerOnBeginRequestShortUrl)
{
    MockRequest request{L"/a"};
    MockResponse response;
    MockContext context{&request, &response};
    MockEventProvider provider;
    const ModuleHandler handler;

    const Verdict verdict = handler.OnBeginRequest(&context, &provider);

    EXPECT_EQ(Verdict::Continue, verdict);
    ASSERT_EQ(1U, response.headers_.size());
}

}
