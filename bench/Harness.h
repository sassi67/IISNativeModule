#ifndef BENCH_HARNESS_H_
#define BENCH_HARNESS_H_

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace bench {

    // Stands in for the IIS boundary: every variant funnels the values it
    // would hand to IIS through the same accumulator, so the only
    // differences measured are the abstraction and the copies, never the
    // "work" at the far end. Also keeps the results observable, so the
    // optimizer can't delete the loop bodies.
    class Sink {
    public:
        auto Consume(const char * text, std::size_t length) -> void {
            accumulator_ += static_cast<std::uint64_t>(text[0]) + length;
        }

        auto Consume(int value) -> void {
            accumulator_ += static_cast<std::uint64_t>(value);
        }

        auto accumulator() const -> std::uint64_t { return accumulator_; }

    private:
        std::uint64_t accumulator_ = 0;
    };

    // Repeats a measurement and reports the minimum (least noise-polluted)
    // alongside the median. Nanoseconds per iteration.
    struct Result {
        double minNs = 0.0;
        double medianNs = 0.0;
    };

    template <typename Callable>
    auto Measure(Callable && body, std::size_t iterations, std::size_t repetitions) -> Result {
        // Warm up: page in code, settle the branch predictor and caches.
        for (std::size_t i = 0; i < iterations / 10 + 1; ++i) {
            body(i);
        }

        std::vector<double> samples;
        samples.reserve(repetitions);
        for (std::size_t rep = 0; rep < repetitions; ++rep) {
            const auto start = std::chrono::steady_clock::now();
            for (std::size_t i = 0; i < iterations; ++i) {
                body(i);
            }
            const auto end = std::chrono::steady_clock::now();
            const double totalNs =
                std::chrono::duration<double, std::nano>(end - start).count();
            samples.push_back(totalNs / static_cast<double>(iterations));
        }

        std::sort(samples.begin(), samples.end());
        Result result;
        result.minNs = samples.front();
        result.medianNs = samples[samples.size() / 2];
        return result;
    }

    // Mimics HTTP_REQUEST::CookedUrl: a non-owning wide pointer plus a
    // length in *bytes*, exactly the shape IIS hands the module.
    struct SimulatedCookedUrl {
        const wchar_t * pAbsPath = nullptr;
        unsigned short AbsPathLength = 0;
    };

    struct SimulatedRawRequest {
        SimulatedCookedUrl CookedUrl;
    };

    inline auto MakeRawRequest(const std::wstring & path) -> SimulatedRawRequest {
        SimulatedRawRequest raw;
        raw.CookedUrl.pAbsPath = path.c_str();
        raw.CookedUrl.AbsPathLength =
            static_cast<unsigned short>(path.length() * sizeof(wchar_t));
        return raw;
    }

}
#endif // BENCH_HARNESS_H_
