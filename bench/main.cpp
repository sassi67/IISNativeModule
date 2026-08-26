#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include <core/ModuleHandler.h>

#include "Harness.h"
#include "Variants.h"

namespace {

    constexpr std::size_t kIterations = 2'000'000;
    constexpr std::size_t kRepetitions = 7;

    struct Row {
        const char * name;
        bench::Result result;
    };

    // Several paths per suite, cycled through, so the optimizer cannot
    // treat the input as loop-invariant. All paths in a suite take the
    // same branch, keeping prediction behaviour comparable across
    // variants.
    auto GenericPaths() -> std::vector<std::wstring> {
        return {
            L"/api/v1/products/12345",
            L"/index.html",
            L"/static/css/site.min.css",
            L"/account/orders/history",
        };
    }

    auto ForbiddenPaths() -> std::vector<std::wstring> {
        return {
            L"/forbidden",
            L"/admin/forbidden",
            L"/api/v1/forbidden",
            L"/secure/area/forbidden",
        };
    }

    auto BuildRaws(const std::vector<std::wstring> & paths)
        -> std::vector<bench::SimulatedRawRequest> {
        std::vector<bench::SimulatedRawRequest> raws;
        raws.reserve(paths.size());
        for (const auto & path : paths) {
            raws.push_back(bench::MakeRawRequest(path));
        }
        return raws;
    }

    auto RunSuite(const char * suiteName, const std::vector<std::wstring> & paths) -> void {
        const auto raws = BuildRaws(paths);
        const std::size_t count = raws.size();

        bench::Sink sink;
        core::ModuleHandler handler;

        std::vector<Row> rows;

        rows.push_back({"A  interface + copy + towlower  (today)",
            bench::Measure([&](std::size_t i) {
                const auto verdict = bench::RunInterfaceCopying(handler, &raws[i % count], &sink);
                sink.Consume(static_cast<int>(verdict));
            }, kIterations, kRepetitions)});

        rows.push_back({"B  template  + copy + towlower",
            bench::Measure([&](std::size_t i) {
                const auto verdict = bench::RunTemplateCopying(&raws[i % count], &sink);
                sink.Consume(static_cast<int>(verdict));
            }, kIterations, kRepetitions)});

        rows.push_back({"C  interface + view + ascii     (proposed)",
            bench::Measure([&](std::size_t i) {
                const auto verdict = bench::RunInterfaceView(&raws[i % count], &sink);
                sink.Consume(static_cast<int>(verdict));
            }, kIterations, kRepetitions)});

        rows.push_back({"D  template  + view + ascii",
            bench::Measure([&](std::size_t i) {
                const auto verdict = bench::RunTemplateView(&raws[i % count], &sink);
                sink.Consume(static_cast<int>(verdict));
            }, kIterations, kRepetitions)});

        rows.push_back({"E  PR #8 replica (raw, no abstraction)",
            bench::Measure([&](std::size_t i) {
                const auto verdict = bench::RunPr8Baseline(&raws[i % count], &sink);
                sink.Consume(static_cast<int>(verdict));
            }, kIterations, kRepetitions)});

        const double floorNs = rows.back().result.minNs;

        std::printf("\n%s  (%zu iterations x %zu repetitions)\n", suiteName, kIterations, kRepetitions);
        std::printf("  %-44s %10s %10s %12s\n", "variant", "min ns", "median ns", "vs PR #8");
        std::printf("  %-44s %10s %10s %12s\n",
            "--------------------------------------------", "---------", "---------", "-----------");
        for (const auto & row : rows) {
            std::printf("  %-44s %10.2f %10.2f %+11.2f\n",
                row.name, row.result.minNs, row.result.medianNs, row.result.minNs - floorNs);
        }

        std::printf("\n  deltas (min):\n");
        std::printf("    A - B  virtual dispatch cost           : %+8.2f ns\n",
            rows[0].result.minNs - rows[1].result.minNs);
        std::printf("    A - C  proposed fix's win (copies+fold) : %+8.2f ns\n",
            rows[0].result.minNs - rows[2].result.minNs);
        std::printf("    C - D  templates after the fix          : %+8.2f ns\n",
            rows[2].result.minNs - rows[3].result.minNs);

        // Keeps every measured call observable so nothing can be elided.
        std::printf("\n  (sink checksum: %llu)\n",
            static_cast<unsigned long long>(sink.accumulator()));
    }

}

auto main(int, char *[]) -> int {
    std::printf("ModuleHandler dispatch micro-benchmark\n");
    std::printf("Release build; timings are per single OnBeginRequest call,\n");
    std::printf("including per-request adapter construction (as production does).\n");

    RunSuite("GENERIC URL  (common path: sets the response header)", GenericPaths());
    RunSuite("FORBIDDEN URL (rejection path: sets status + error)", ForbiddenPaths());

    return 0;
}
