#pragma once

#include <iostream>
#include <cstdlib>
#include <functional>
#include <memory>

#include <opencv2/core.hpp>

#include <lib/utils.hpp>

std::function<void(uchar&)> create_gamma_curve_lambda(std::shared_ptr<Options> &options);

void apply_curve_non_accelerated(std::shared_ptr<cv::Mat> &img, std::shared_ptr<Options> &options);

std::shared_ptr<cv::Mat> load_input_image(std::shared_ptr<Options> &options);
