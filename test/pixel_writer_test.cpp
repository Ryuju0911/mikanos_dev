#include <gtest/gtest.h>

#include "frame_buffer_config.hpp"
#include "graphics.hpp"

TEST(PixelWriterTest, WritePixel) {
  char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
  PixelWriter *pixel_writer;

  FrameBufferConfig frame_buffer_config;
  frame_buffer_config.pixels_per_scan_line = 100;
  frame_buffer_config.pixel_format = kPixelRGBResv8BitPerColor;

  pixel_writer = new RGBResv8BitPerColorPixelWriter{frame_buffer_config};
  int x = 50;
  int y = 50;
  pixel_writer->Write(x, y, {255, 255, 255});

  auto p = frame_buffer_config.frame_buffer
    + 4 * (frame_buffer_config.pixels_per_scan_line * y + x);
  ASSERT_EQ(p[0], 255);
  ASSERT_EQ(p[1], 255);
  ASSERT_EQ(p[2], 255);
}