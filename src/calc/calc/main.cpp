#include <calclib/sum.hpp>

#include <cxxopts.hpp>

#include <iostream>

int main(int argc, char **argv) {
    cxxopts::Options options("sum");

    // clang-format off
    options.add_options()
        ("first", "first param", cxxopts::value<float>())
        ("second", "second param", cxxopts::value<float>());
    // clang-format on

    const auto result = options.parse(argc, argv);

    if ((result.count("first") != 1) || (result.count("second") != 1)) {
        std::cout << options.help() << "\n";
        return 0;
    }

    const auto first = result["first"].as<float>();
    const auto second = result["second"].as<float>();

    std::cout << calclib::sum_floats(first, second) << std::endl;
    return 0;
}

