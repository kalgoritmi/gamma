#include <iostream>
#include <chrono>
#include <functional>
#include <memory>
#include <bit>
#include <immintrin.h>
#include <cstdint>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <lib/utils.hpp>
#include <lib/pipeline.hpp>

#define MANTISSA_MASK 0x007FFFFF
#define EXPONENT_ONE 0x3F800000
#define LOG_2 0.6931471f

__m256 _mm256_log(__m256 v, int n = 20) {
    __m256 x = _mm256_sub_ps(v, _mm256_set1_ps(1.0f));

    __m256 acc = x;
    __m256 _x = x;
    for (int i = 2; i <= n; i++) {
        _x = _mm256_mul_ps(_x, x);
        acc = _mm256_add_ps(
            acc,
            _mm256_div_ps(
                _x,
                _mm256_set1_ps(static_cast<float>(i % 2 ? i : -i))
            )
        );
    }

    return acc;
}

__m256 vec_ieee_mantissa(__m256 v) {
    __m256i integer_bits = _mm256_castps_si256(v);

    __m256i mantissa_mask = _mm256_set1_epi32(MANTISSA_MASK);

    __m256i mantissa_bits = _mm256_and_si256(integer_bits, mantissa_mask);

    __m256i exponent_one = _mm256_set1_epi32(EXPONENT_ONE);

    mantissa_bits = _mm256_or_si256(mantissa_bits, exponent_one);

    return _mm256_castsi256_ps(mantissa_bits);
}

__m256 vec_ieee_exponent(__m256 v) {
    __m256i integer_bits = _mm256_castps_si256(v);

    __m256i left_shifted = _mm256_srli_epi32(integer_bits, 23);

    __m256i exponent_mask = _mm256_set1_epi32(0xFF);

    __m256i exponent_bits = _mm256_and_si256(left_shifted, exponent_mask);

    __m256 exponent = _mm256_cvtepi32_ps(exponent_bits);

    return _mm256_sub_ps(exponent, _mm256_set1_ps(127.0f));

}

__m256 vec_log(__m256 v, int n = 20) {
    __m256 mantissa = vec_ieee_mantissa(v);

    __m256 exp_log_2 = _mm256_mul_ps(vec_ieee_exponent(v), _mm256_set1_ps(LOG_2));

    __m256 log_exp = _mm256_log(mantissa, n);

    return log_exp + exp_log_2;
}

__m256 vec_exp(__m256 b, float power, int iterations = 20) {
    __m256 p = _mm256_set1_ps(power);

    __m256 constant = _mm256_add_ps(
        _mm256_mul_ps(p, vec_log(b)),
        _mm256_set1_ps(1.0f)
    );

    __m256 result = b;
    for (int i = static_cast<int>(power); --i > 0;) {
        result = _mm256_mul_ps(result, b);
    }

    while(--iterations > 0) {
        result = _mm256_mul_ps(result, _mm256_sub_ps(constant, vec_log(result)));
    }

    return result;
}

__m256 curve(__m256 v, float g, float t) {
    __m256 mask = _mm256_cmp_ps(v, _mm256_set1_ps(t), _CMP_LT_OS); // Create a mask for v < t

// Calculate the two branches without conditional operations.
    __m256 branch1 = _mm256_mul_ps(
            _mm256_mul_ps(_mm256_set1_ps(255.0f), _mm256_div_ps(v, _mm256_set1_ps(t))),
            vec_exp(_mm256_div_ps(_mm256_set1_ps(t), _mm256_set1_ps(255.0f)), g)
    );

    __m256 branch2 = _mm256_mul_ps(
            _mm256_set1_ps(255.0f), vec_exp(_mm256_div_ps(v, _mm256_set1_ps(255.0f)), g)
    );

// Blend the results using bitwise AND/OR operations.
    __m256 result = _mm256_or_ps(
            _mm256_and_ps(mask, branch1),          // When mask is true, select branch1
            _mm256_andnot_ps(mask, branch2)        // When mask is false, select branch2
    );

    return result;
}

void apply(float* pixels, size_t sz, std::shared_ptr<Options> options) {
    // todo: memcopy in a memory space which is a multiple of 8
    size_t batch_sz = sz / 8 + static_cast<size_t>(sz % 8 > 0);

    for (size_t batch = 0; batch < batch_sz; batch += 1)
        _mm256_storeu_ps(
            pixels + 8*batch,
            curve(
                _mm256_loadu_ps(pixels + 8*batch),
                options->g_val,
                static_cast<float>(options->t_val)
            )
        );
}

int main(int argc, char *args[]) {
    auto options = parse_arguments(argc, args);

    auto img = load_input_image(options);
    auto duration1 = measure_pipeline<std::chrono::microseconds, std::function<void()>>([&options, &img](){
        apply_curve_non_accelerated(img, options);
    });
    cv::imwrite(options->out_img_path, *img);

    //float* v = new float[17]{1.5625, 1.834, 2.0, 4.0, 5.0, 6.0, 7.0, 8.0, 1.5625, 1.834, 2.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

    //apply(v, 17);

    cv::Mat img_32f;
    load_input_image(options)->convertTo(img_32f, CV_32F);
    auto duration2 = measure_pipeline<std::chrono::microseconds, std::function<void()>>([&options, &img_32f](){
        float* _img_32f = reinterpret_cast<float*>(img_32f.data);
        size_t size = img_32f.cols * img_32f.rows * img_32f.channels();

        apply(_img_32f, size, options);
    });
    cv::Mat _img;
    img_32f.convertTo(_img, CV_8U, 255.0);
    cv::imwrite("test-" + options->out_img_path, _img);

    //for (size_t i = 0; i < 24; i++)
    //    std::cout << v[i] << " " << std::endl;

    std::cout << "Processing took: " << duration1
              << ", image saved as " << options->out_img_path << "." << std::endl;
    std::cout << "Processing took: " << duration2
              << ", image saved as " << "test-" + options->out_img_path << "." << std::endl;
}
