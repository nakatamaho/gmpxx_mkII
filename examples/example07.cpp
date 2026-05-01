// SPDX-License-Identifier: BSD-2-Clause

#include "gmpxx_mkII.h"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpfc_class;

struct rgb {
    int r;
    int g;
    int b;
};

struct render_config {
    int width = 96;
    int height = 40;
    int max_iterations = 160;
    mp_bitcnt_t precision = 192;
    char const* center_real = "0";
    char const* center_imag = "0";
    char const* scale = "3.5";
    char const* y_pixel_aspect = "2";
};

mpfc_class make_complex(mpf_class const& real_value,
                        mpf_class const& imag_value) {
    return mpfc_class(real_value, imag_value);
}

int escape_iterations(mpfc_class const& c, int max_iterations,
                      mp_bitcnt_t precision) {
    mpfc_class z(mpf_class(0, precision), mpf_class(0, precision));
    mpf_class escape_radius_squared(4, precision);

    for (int iter = 0; iter < max_iterations; ++iter) {
        z = z * z + c;
        if (gmpxx::norm(z) > escape_radius_squared) {
            return iter + 1;
        }
    }
    return max_iterations;
}

rgb color_for(int iterations, int max_iterations) {
    if (iterations >= max_iterations) {
        return {0, 0, 0};
    }

    int t = (255 * iterations) / max_iterations;
    int inv = 255 - t;
    return {
        (9 * t * inv) / 1024,
        (7 * t * t) / 2048,
        inv,
    };
}

void validate_config(render_config const& config) {
    if (config.width <= 0 || config.height <= 0 ||
        config.max_iterations <= 0) {
        throw std::invalid_argument("image dimensions and iterations must be positive");
    }
}

struct render_state {
    mpf_class center_real;
    mpf_class center_imag;
    mpf_class scale;
    mpf_class width;
    mpf_class height;
    mpf_class y_pixel_aspect;
    mpf_class real_span;
    mpf_class imag_span;
    mpf_class half;
};

render_state make_render_state(render_config const& config) {
    gmpxx::gmpxx_defaults::set_initial_default_prec(config.precision);

    mpf_class center_real(config.center_real);
    mpf_class center_imag(config.center_imag);
    mpf_class scale(config.scale);
    mpf_class y_pixel_aspect(config.y_pixel_aspect);
    mpf_class width(config.width);
    mpf_class height(config.height);
    mpf_class aspect = height / width;
    mpf_class real_span = scale;
    mpf_class imag_span = scale * aspect * y_pixel_aspect;
    mpf_class half("0.5");

    return {center_real, center_imag, scale, width, height, y_pixel_aspect,
            real_span, imag_span, half};
}

int iterations_at(int x, int y, render_config const& config,
                  render_state const& state) {
    mpf_class y_pos =
        (mpf_class(y) + state.half) / state.height - state.half;
    mpf_class imag = state.center_imag - y_pos * state.imag_span;

    mpf_class x_pos =
        (mpf_class(x) + state.half) / state.width - state.half;
    mpf_class real = state.center_real + x_pos * state.real_span;
    mpfc_class c = make_complex(real, imag);

    return escape_iterations(c, config.max_iterations, config.precision);
}

void write_ascii(std::ostream& out, render_config const& config) {
    validate_config(config);
    render_state state = make_render_state(config);

    char const* palette = "@%#*+=-:. ";
    constexpr int palette_size = 10;

    out << "gmpxx_mkII example07 Mandelbrot deep zoom\n";
    out << "center=(" << state.center_real << "," << state.center_imag
        << "), scale=" << state.scale
        << ", y_pixel_aspect=" << state.y_pixel_aspect
        << ", precision=" << config.precision
        << ", max_iterations=" << config.max_iterations << "\n\n";

    for (int y = 0; y < config.height; ++y) {
        for (int x = 0; x < config.width; ++x) {
            int iterations = iterations_at(x, y, config, state);
            int index = ((palette_size - 1) * iterations) /
                        config.max_iterations;
            out << palette[index];
        }
        out << '\n';
    }
}

void write_ppm(std::ostream& out, render_config const& config) {
    validate_config(config);
    render_state state = make_render_state(config);

    out << "P3\n";
    out << "# gmpxx_mkII example07 Mandelbrot deep zoom\n";
    out << "# center=(" << state.center_real << "," << state.center_imag
        << ")\n";
    out << "# scale=" << state.scale << ", precision=" << config.precision
        << ", max_iterations=" << config.max_iterations << '\n';
    out << config.width << ' ' << config.height << "\n255\n";

    for (int y = 0; y < config.height; ++y) {
        for (int x = 0; x < config.width; ++x) {
            int iterations = iterations_at(x, y, config, state);
            rgb color = color_for(iterations, config.max_iterations);
            out << color.r << ' ' << color.g << ' ' << color.b;
            out << (x + 1 == config.width ? '\n' : ' ');
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    render_config config;

    if (argc > 3 || (argc >= 2 && std::string(argv[1]) != "--ppm")) {
        std::cerr << "usage: " << argv[0] << " [--ppm [output.ppm]]\n";
        return 2;
    }

    try {
        if (argc >= 2) {
            config.width = 320;
            config.height = 240;
            config.y_pixel_aspect = "1";
            if (argc == 3) {
                std::ofstream file(argv[2]);
                if (!file) {
                    std::cerr << "failed to open output file: " << argv[2]
                              << '\n';
                    return 1;
                }
                write_ppm(file, config);
            } else {
                write_ppm(std::cout, config);
            }
        } else {
            write_ascii(std::cout, config);
        }
    } catch (std::exception const& ex) {
        std::cerr << "error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
