#include "RPNG.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
namespace superpunto {

void png_user_warn(png_structp ctx, png_const_charp str) {
  fprintf(stderr, "libpng: warning: %s\n", str);
}

void png_user_error(png_structp ctx, png_const_charp str) {
  fprintf(stderr, "libpng: error: %s\n", str);
}

bool savePNG(const char *fileName, const unsigned char *pixels, int width,
             int height, int colorType) {
  FILE *fp = std::fopen(fileName, "wb");
  if (!fp) {
    std::perror("Failed to open PNG file");
    return false;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    std::fclose(fp);
    std::cerr << "png_create_write_struct failed\n";
    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    std::fclose(fp);
    std::cerr << "png_create_info_struct failed\n";
    return false;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    std::fclose(fp);
    std::cerr << "libpng error writing PNG\n";
    return false;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, colorType,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr, info_ptr);
  png_set_packing(png_ptr);

  std::vector<png_bytep> row_pointers(height);
  for (int i = 0; i < height; ++i) {
    row_pointers[i] =
        const_cast<png_bytep>(pixels + (height - i - 1) * width * 4);
  }

  png_write_image(png_ptr, row_pointers.data());
  png_write_end(png_ptr, nullptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  std::fclose(fp);
  return true;
}
} // namespace superpunto
