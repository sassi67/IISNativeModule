#include <utils/smoke.h>
#include <fmt/core.h>

auto main(int, char *[]) -> int {
    auto message = utils::Smoke::getHello();
    fmt::print("{}", message);
    return 0;
}