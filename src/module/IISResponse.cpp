#include "IISResponse.h"

namespace iis {

auto IISResponse::SetStatus(
    int statusCode,
    std::string_view reason,
    std::int32_t errorStatus) -> void {
    // reason.data() is forwarded directly rather than copied into a
    // std::string: core::Response requires these views to be
    // null-terminated for exactly this reason.
    // fTrySkipCustomErrors: keep our own reason phrase instead of
    // letting the custom-errors module replace the response.
    pResponse_->SetStatus(
        static_cast<USHORT>(statusCode),
        reason.data(),
        0,
        static_cast<HRESULT>(errorStatus),
        nullptr,
        TRUE);
}

auto IISResponse::SetHeader(std::string_view name, std::string_view value) -> void {
    pResponse_->SetHeader(
        name.data(),
        value.data(),
        static_cast<USHORT>(value.length()),
        TRUE);
}

}
