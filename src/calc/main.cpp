#include <calclib/sum.hpp>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

#include <iostream>

int main(int argc, char **argv) {
    CLI::App app{"Sum of two floats"};

    float first = 0;
    float second = 0;
    app.add_option("-f,--first", first, "First float");
    app.add_option("-s,--second", second, "Second float");

    CLI11_PARSE(app, argc, argv);

    std::cout << calclib::sum_floats(first, second) << std::endl;
    return 0;
}

