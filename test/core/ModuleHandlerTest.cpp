#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <core/ModuleHandler.h>

namespace core {

namespace {

    class MockRequest : public IRequest {
    public:
        explicit MockRequest(std::wstring url) : rawRequest_{std::move(url)} {}

        auto GetRawHttpRequest() const -> const RawHttpRequest * override {
            return &rawRequest_;
        }

    private:
        RawHttpRequest rawRequest_;
    };

    // Records every call so the tests can assert on what the handler
    // did to the response.
    class MockResponse : public IResponse {
    public:
        auto SetStatus(
            int statusCode,
            std::string_view reason,
            std::int32_t errorStatus) -> void override {
            statusCode_ = statusCode;
            reason_ = std::string{reason};
            errorStatus_ = errorStatus;
        }

        auto SetHeader(std::string_view name, std::string_view value) -> void override {
            headers_.emplace_back(std::string{name}, std::string{value});
        }

        int statusCode_ = 0;
        std::string reason_;
        std::int32_t errorStatus_ = 0;
        std::vector<std::pair<std::string, std::string>> headers_;
    };

    class MockContext : public IContext {
    public:
        MockContext(IRequest * request, IResponse * response)
            : request_{request}, response_{response} {}

        auto GetRequest() -> IRequest * override { return request_; }
        auto GetResponse() -> IResponse * override { return response_; }

    private:
        IRequest * request_;
        IResponse * response_;
    };

    class MockEventProvider : public IEventProvider {
    public:
        auto SetErrorStatus(std::int32_t errorStatus) -> void override {
            errorStatus_ = errorStatus;
        }

        std::int32_t errorStatus_ = 0;
    };

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
    ModuleHandler handler;

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
    ModuleHandler handler;

    const Verdict verdict = handler.OnBeginRequest(&context, &provider);

    EXPECT_EQ(Verdict::Continue, verdict);
    EXPECT_EQ(0, response.statusCode_);
    EXPECT_EQ(0, provider.errorStatus_);
    ASSERT_EQ(1U, response.headers_.size());
    EXPECT_EQ("X-IISRequestLevelModule", response.headers_[0].first);
    EXPECT_EQ("1", response.headers_[0].second);
}

}
