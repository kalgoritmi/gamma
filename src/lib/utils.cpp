#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <type_traits>

#include <opencv2/core.hpp>

#include <lib/utils.hpp>

std::shared_ptr<Options> parse_arguments(int argc, char *args[]) {
    auto options = std::make_shared<Options>();

    if (argc != 1 + get_type_length<Options>()) {
        std::cerr << "Usage: gamma <input_image> <t_value> <g_value> <output_image>" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (--argc > 0) {
        options->set(argc - 1, args[argc]);
    }

    return options;
}

std::ostream& operator<<(std::ostream& os, const std::chrono::duration<double, std::milli>& duration) {
    os << std::fixed << std::setprecision(3) << duration.count() << "ms" ;
    return os;
}
