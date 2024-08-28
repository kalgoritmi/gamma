#pragma once

#include <iostream>
#include <compare>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <reflect>
#include <string>
#include <type_traits>

typedef struct Options {
    std::string in_img_path;
    int t_val;
    double g_val;
    std::string out_img_path;

    template <typename T>
    void set(int idx, T val);

    int operator<=>(const Options&) const = default;
} Options;

template <typename T>
void Options::set(int idx, T val) {
    switch(idx) {
        case 0:
            this->in_img_path = val;
            break;
        case 1:
            this->t_val = atoi(val);
            break;
        case 2:
            this->g_val = atof(val);
            break;
        case 3:
            this-> out_img_path = val;
            break;
        default:
            break;
    }
}

template <
    typename T,
    typename = std::enable_if<!std::is_reference<T>::value>::type,
    typename = std::enable_if<!std::is_pointer<T>::value>::type
>
int get_type_length() {
    return reflect::size(T());
}

std::shared_ptr<Options> parse_arguments(int argc, char *args[]);

template<
    typename Duration = std::chrono::microseconds,
    typename Fn,
    typename... Args,
    typename = std::enable_if_t<std::is_invocable_v<Fn, Args...>, void>
>
[[nodiscard]] std::chrono::duration<double, std::milli> measure_pipeline(Fn&& fn, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();

    std::forward<Fn>(fn)(std::forward<Args>(args)...);

    auto duration = std::chrono::duration_cast<Duration>(
            std::chrono::high_resolution_clock::now() - start
    );

    return std::chrono::duration<double, std::milli>(duration);
}

std::ostream& operator<<(std::ostream& os, const std::chrono::duration<double, std::milli>& duration);
