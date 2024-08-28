#include <iostream>
#include <cstdlib>
#include <functional>
#include <memory>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <lib/pipeline.hpp>

std::function<void(uchar&)> create_gamma_curve_lambda(std::shared_ptr<Options> &options) {
    return [&options](uchar &subpixel) -> void {
        subpixel = subpixel < options->t_val ?
           255 * std::pow(static_cast<double>(options->t_val) / 255, options->g_val) * (static_cast<double>(subpixel) / options->t_val) :
           255 * std::pow(static_cast<double>(subpixel) / 255, options->g_val);
    };
}

void apply_curve_non_accelerated(std::shared_ptr<cv::Mat> &img, std::shared_ptr<Options> &options) {
    auto gamma_curve = create_gamma_curve_lambda(options);

    img->forEach<cv::Vec3b>([&gamma_curve](cv::Vec3b &pixel, [[maybe_unused]] const int *position) -> void {
        gamma_curve(pixel[0]);
        gamma_curve(pixel[1]);
        gamma_curve(pixel[2]);
    });
}

std::shared_ptr<cv::Mat> load_input_image(std::shared_ptr<Options> &options) {
    auto img = std::make_shared<cv::Mat>(cv::imread(options->in_img_path, cv::IMREAD_COLOR));

    if (img->empty()) {
        std::cerr << "ABORTING Invalid image " << options->in_img_path << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Successfully loaded image " << options->in_img_path << "." << std::endl;
    }

    return img;
}

