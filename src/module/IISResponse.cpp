#include "IISResponse.h"

#include <string>

namespace iis {

auto IISResponse::SetStatus(
    int statusCode,
    std::string_view reason,
    std::int32_t errorStatus) -> void {
    const std::string reasonString{reason};
    // fTrySkipCustomErrors: keep our own reason phrase instead of
    // letting the custom-errors module replace the response.
    pResponse_->SetStatus(
        static_cast<USHORT>(statusCode),
        reasonString.c_str(),
        0,
        static_cast<HRESULT>(errorStatus),
        nullptr,
        TRUE);
}

auto IISResponse::SetHeader(std::string_view name, std::string_view value) -> void {
    const std::string nameString{name};
    const std::string valueString{value};
    pResponse_->SetHeader(
        nameString.c_str(),
        valueString.c_str(),
        static_cast<USHORT>(valueString.length()),
        TRUE);
}

}
