#include <iostream>
#include <memory>
#include <string>

#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <lib/pipeline.hpp>
#include <lib/utils.hpp>

using namespace std::literals::string_literals;

class GAMMA_PIPELINE : public ::testing::Test {
public:
    std::shared_ptr<Options> options;
    std::shared_ptr<cv::Mat> img_in;
    std::shared_ptr<cv::Mat> img_out;

    void SetUp() override {
        options = std::make_shared<Options>();

        options->in_img_path = "dummy_input.jpg"s;
        options->out_img_path = "dummy_output.jpg"s;

        options->g_val = .5d;
        options->t_val = 20;

        img_in = std::make_shared<cv::Mat>(2, 2, CV_8UC3);

        img_in->at<cv::Vec3b>(0, 0) = {0, 10, 255};
        img_in->at<cv::Vec3b>(0, 1) = {0, 20, 10};
        img_in->at<cv::Vec3b>(1, 0) = {255, 10, 0};
        img_in->at<cv::Vec3b>(1, 1) = {255, 10, 20};

        img_out = std::make_shared<cv::Mat>(2, 2, CV_8UC3);

        img_out->at<cv::Vec3b>(0, 0) = {0, 35, 255};
        img_out->at<cv::Vec3b>(0, 1) = {0, 71, 35};
        img_out->at<cv::Vec3b>(1, 0) = {255, 35, 0};
        img_out->at<cv::Vec3b>(1, 1) = {255, 35, 71};
    }
};

/*  Test targeting gamma curve, both branches on some known values.
 *  The function created by teh factory, is stateful and does not return anything
 *  instead it mutates the reference it acts upon.
 */
TEST_F(GAMMA_PIPELINE, create_gamma_curve_lambda) {
    auto gamma_curve_callable = create_gamma_curve_lambda(options);

    uchar value;

    // value >= threshold branch test
    value = static_cast<uchar>(255);
    gamma_curve_callable(value);
    EXPECT_EQ(static_cast<uchar>(255), value);

    value = static_cast<uchar>(20);
    gamma_curve_callable(value);
    EXPECT_EQ(static_cast<uchar>(71), value);

    // value < threshold branch test
    value = static_cast<uchar>(10);
    gamma_curve_callable(value);
    EXPECT_EQ(static_cast<uchar>(35), value);
}

// Test targeting apply_curve_non_accelerated, on a 2 by 2, 3 Channel x 8B / pixel Mat
TEST_F(GAMMA_PIPELINE, apply_curve_non_accelerated) {
    apply_curve_non_accelerated(img_in, options);
    img_in->forEach<cv::Vec3b>([this](cv::Vec3b& in_pixel, const int* position) {
        EXPECT_EQ(this->img_out->at<cv::Vec3b>(position[0], position[1]), in_pixel);
    });
}

// test image loading, just the dimensions of the mat
// could move imread to an interface and mock it to test values as well
TEST_F(GAMMA_PIPELINE, load_input_image) {
    options->in_img_path = "../data/aerial-view.jpg"s;
    auto img = load_input_image(options);

    EXPECT_EQ(1281, img->rows);
    EXPECT_EQ(1920, img->cols);
    EXPECT_EQ(3, img->channels());
}

// test image loading, should exit with failure code on invalid image path
TEST_F(GAMMA_PIPELINE, load_empty_input_image) {
    options->in_img_path = ""s;
    ASSERT_EXIT(load_input_image(options), testing::ExitedWithCode(EXIT_FAILURE), "ABORTING Invalid image ");
}

