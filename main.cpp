#include <cstdint>
#include <iostream>
#include <vips/vips8>
#include <omp.h>

void mandelbrot(const std::complex<double>& c, uint16_t limit, uint8_t* out) {
    std::complex<double> z = {0, 0};
    int iter = 0;
    while (iter < limit) {
        double r = z.real();
        double i = z.imag();
        if((r * r + i * i) > 4)
            break;
        z = z * z + c;
        iter++;
    }

    if (iter == limit) {
        out[0] = out[1] = out[2] = 0;
        return;
    }

    out[0] = (255 * iter) % 256;
    out[1] = (255 * (iter % 64)) % 256;
    out[2] = (255 * (iter / 4)) % 256;
}

bool saveImage(const std::vector<std::vector<std::vector<uint8_t>>>& img, const std::string& filename) {
    if (VIPS_INIT("mandelbrot"))
        return false;

    int height = img.size();
    int width = img[0].size();
    int channels = 3;

    std::vector<uint8_t> data;
    data.reserve(width * height * channels);
    for (const auto& row : img) {
        for (const auto& pixel : row) {
            data.insert(data.end(), pixel.begin(), pixel.end());
        }
    }

    VipsImage* image = vips_image_new_from_memory(data.data(), data.size(), width, height, channels, VIPS_FORMAT_UCHAR);
    if (!image) {
        vips_error_exit(nullptr);
        return false;
    }

    if (vips_pngsave(image, filename.c_str(), nullptr)) {
        g_object_unref(image);
        vips_shutdown();
        return false;
    }

    g_object_unref(image);
    vips_shutdown();

    return true;
}

int main() {
    const int WIDTH = 4096 * 1/*.5*/;
    const int HEIGHT = 2160 * 1/*.5*/;
    const int LIMIT = 512;
    const double ZOOM = 4.0;

    std::vector img(HEIGHT, std::vector(WIDTH, std::vector<uint8_t>(3, 0)));

    omp_set_num_threads(omp_get_max_threads());

    double start_time = omp_get_wtime();
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            std::complex<double> c(
                (i - WIDTH / 2.0) * ZOOM / WIDTH,
                (j - HEIGHT / 2.0) * ZOOM / HEIGHT
            );

            mandelbrot(c, LIMIT, &*img[j][i].begin());
        }
    }
    double end_time = omp_get_wtime();
    std::cout << "Time (seq): " << end_time - start_time << " seconds\n";

    /*start_time = omp_get_wtime();
    #pragma omp parallel for num_threads(8) schedule(dynamic)
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            std::complex<double> c(
                (i - WIDTH / 2.0) * ZOOM / WIDTH,
                (j - HEIGHT / 2.0) * ZOOM / HEIGHT
            );

            mandelbrot(c, LIMIT, &*img[j][i].begin());
        }
    }
    end_time = omp_get_wtime();
    std::cout << "Time (par): " << end_time - start_time << " seconds\n";*/

    /*if (saveImage(img, "output.png")) {
        std::cout << "PNG saved successfully." << std::endl;
    } else {
        std::cerr << "Failed to save PNG." << std::endl;
    }*/

    return 0;
}
