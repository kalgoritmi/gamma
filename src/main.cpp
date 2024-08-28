#include <iostream>
#include <chrono>
#include <functional>
#include <memory>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <lib/utils.hpp>
#include <lib/pipeline.hpp>

int main(int argc, char *args[]) {
    auto options = parse_arguments(argc, args);

    auto duration = measure_pipeline<std::chrono::microseconds, std::function<void()>>([&options](){
        auto img = load_input_image(options);
        apply_curve_non_accelerated(img, options);
        cv::imwrite(options->out_img_path, *img);
    });

    std::cout << "Processing took: " << duration
              << ", image saved as " << options->out_img_path << "." << std::endl;
}
