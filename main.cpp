#include <cstdint>
#include <iostream>
#include <vips/vips8>
#include <omp.h>

void mandelbrot(const double cr, const double ci, uint16_t limit, uint8_t* out) {
    double zr = 0, zi = 0;
    int iter = 0;
    while (iter < limit) {
        const double zrs = zr * zr;
        const double zis = zi * zi;
        if((zrs + zis) > 4)
            break;

        const double tmp = zr * zi;
        zr = zrs - zis + cr;
        zi = tmp + tmp + ci;
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

bool saveImage(const uint8_t* data, const int32_t width, const int32_t height, const std::string& filename) {
    if (VIPS_INIT("mandelbrot"))
        return false;

    constexpr int channels = 3;
    VipsImage* image = vips_image_new_from_memory(data, width * height * channels, width, height, channels, VIPS_FORMAT_UCHAR);
    if (!image) {
        vips_error_exit(nullptr);
        return false;
    }

    const bool imageSaved = vips_pngsave(image, filename.c_str(), nullptr) == 0;
    g_object_unref(image);
    vips_shutdown();
    return imageSaved;
}

// single-thread opt: 53 -> 44 -> 32 -> (somehow) 12  (seconds)
// multi-thread opt:  10 -> 7  -> 4  -> (somehow) 2   (seconds)
int main() {
    const int WIDTH = 4096 * 1.5;
    const int HEIGHT = 2160 * 1.5;
    const int LIMIT = 512;
    const double ZOOM = 4.0;

    auto* img = (uint8_t*)malloc(HEIGHT * WIDTH * 3);
    omp_set_num_threads(omp_get_max_threads());

    double start_time = omp_get_wtime();
    for (int j = 0; j < HEIGHT; j++) {
        uint8_t* row_start = img + j * WIDTH * 3;
        int offset = 0;
        for (int i = 0; i < WIDTH; i++) {
            const double cr = (i - WIDTH / 2.0) * ZOOM / WIDTH;
            const double ci = (j - HEIGHT / 2.0) * ZOOM / HEIGHT;
            mandelbrot(cr, ci, LIMIT, row_start + offset);
            offset += 3;
        }
    }
    double end_time = omp_get_wtime();
    std::cout << "Time (seq): " << end_time - start_time << " seconds\n";

    start_time = omp_get_wtime();
    #pragma omp parallel for num_threads(8) schedule(dynamic)
    for (int j = 0; j < HEIGHT; j++) {
        uint8_t* row_start = img + j * WIDTH * 3;
        int offset = 0;
        for (int i = 0; i < WIDTH; i++) {
            const double cr = (i - WIDTH / 2.0) * ZOOM / WIDTH;
            const double ci = (j - HEIGHT / 2.0) * ZOOM / HEIGHT;
            mandelbrot(cr, ci, LIMIT, row_start + offset);
            offset += 3;
        }
    }
    end_time = omp_get_wtime();
    std::cout << "Time (par): " << end_time - start_time << " seconds\n";

    if (saveImage(img, WIDTH, HEIGHT, "output.png")) {
        std::cout << "PNG saved successfully." << std::endl;
    } else {
        std::cerr << "Failed to save PNG." << std::endl;
    }

    free(img);
    return 0;
}
