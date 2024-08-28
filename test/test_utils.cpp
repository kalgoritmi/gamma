#include <iostream>
#include <chrono>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <typeinfo>

#include <gtest/gtest.h>

#include <lib/utils.hpp>
#include <lib/pipeline.hpp>

using namespace std::literals::string_literals;

struct GAMMA_UTILS : public testing::Test {
    std::shared_ptr<Options> options;

    void SetUp() override {
        options = std::make_shared<Options>();

        options->in_img_path = "dummy_input.jpg"s;
        options->out_img_path = "dummy_output.jpg"s;

        options->g_val = .5d;
        options->t_val = 50;
    }
};

/*  Test targeting parse arguments, it should parse 4 arguments into an Options inst:
 *      - input_path, string
 *      - threshold, int
 *      - gamma, double
 *      - output_path, string
 *  and verify their values.
 */
TEST_F(GAMMA_UTILS, parse_arguments) {
    char* args[] = {
            (char*)"gamma",
            (char*)"dummy_input.jpg",
            (char*)"50",
            (char*)"0.5",
            (char*)"dummy_output.jpg"
    };

    EXPECT_EQ(*options, *parse_arguments(5, args));
    EXPECT_EQ(typeid(options->in_img_path), typeid(std::string));
    EXPECT_EQ(typeid(options->t_val), typeid(int));
    EXPECT_EQ(typeid(options->g_val), typeid(double));
    EXPECT_EQ(typeid(options->out_img_path), typeid(std::string));

    ASSERT_EXIT(
        parse_arguments(4, args),
        testing::ExitedWithCode(EXIT_FAILURE),
        "Usage: gamma <input_image> <t_value> <g_value> <output_image>"
    );
}

/*  Test targeting measure_pipeline, this accepts a measurement chrono resolution and a callable
 *  but it returns teh duration converted to milliseconds with decimal places.
 *  The test checks that the duration is at least as long as the sleep time inside the callable.
 */
TEST_F(GAMMA_UTILS, measure_pipeline) {
    auto duration = measure_pipeline<std::chrono::microseconds,std::function<void()>>([](){
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    });

    EXPECT_LT(1, duration.count());
}

//  Test targeting measure_pipeline return type operator<< overload
TEST_F(GAMMA_UTILS, test_ostream_overload_for_duration_milli) {
    std::chrono::duration<double, std::milli> duration(1.1989);

    std::ostringstream oss;
    oss << duration;

    EXPECT_EQ("1.199ms"s, oss.str());
}
