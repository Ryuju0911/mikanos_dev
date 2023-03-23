#include "console.hpp"

#include <cstring>

#include "font.hpp"
#include "graphics.hpp"
#include "layer.hpp"

// #@@range_begin(constructor)
Console::Console(const PixelColor& fg_color, const PixelColor& bg_color)
    : writer_{nullptr}, window_{}, fg_color_{fg_color}, bg_color_{bg_color},
      buffer_{}, cursor_row_{0}, cursor_column_{0}, layer_id_{0} {
}
// #@@range_end(constructor)

// #@@range_begin(put_string)
void Console::PutString(const char* s) {
  while (*s) {
    if (*s == '\n') {
      Newline();
    } else if (cursor_column_ < kColumns - 1) {
      WriteAscii(*writer_, Vector2D<int>{8 * cursor_column_, 16 * cursor_row_}, *s, fg_color_);
      buffer_[cursor_row_][cursor_column_] = *s;
      ++cursor_column_;
    }
    ++s;
  }
  if (layer_manager) {
    layer_manager->Draw(layer_id_);
  }
}
// #@@range_end(put_string)

// #@@range_begin(setwriter)
void Console::SetWriter(PixelWriter* writer) {
  if (writer == writer_) {
    return;
  }
  writer_ = writer;
  window_.reset();
  Refresh();
}
// #@@range_end(setwriter)

// #@@range_begin(setwindow)
void Console::SetWindow(const std::shared_ptr<Window> &window) {
  if (window == window_) {
    return;
  }
  window_ = window;
  writer_ = window->Writer();
  Refresh();
}
// #@@range_end(setwindow)

// #@@range_begin(set_layer_id)
void Console::SetLayerID(unsigned int layer_id) {
  layer_id_ = layer_id;
}
// #@@range_end(set_layer_id)

unsigned int Console::LayerID() const {
  return layer_id_;
}

// #@@range_begin(newline)
void Console::Newline() {
  cursor_column_ = 0;
  if (cursor_row_ < kRows - 1) {
    ++cursor_row_;
    return;
  }

  if (window_) {
    Rectangle<int> move_src{{0, 16}, {8 * kColumns, 16 * (kRows - 1)}};
    window_->Move({0, 0}, move_src);
    FillRectangle(*writer_, {0, 16 * (kRows - 1)}, {8 * kColumns, 16}, bg_color_);
  } else {
    FillRectangle(*writer_, {0, 0}, {8 * kColumns, 16 * kRows}, bg_color_);
    for (int row = 0; row < kRows - 1; ++row) {
      memcpy(buffer_[row], buffer_[row + 1], kColumns + 1);
      WriteString(*writer_, Vector2D<int>{0, 16 * row}, buffer_[row], fg_color_);
  }
    memset(buffer_[kRows - 1], 0, kColumns + 1);
  }
}
// #@@range_end(newline)

// #@@range_begin(refresh)
void Console::Refresh() {
  FillRectangle(*writer_, {0, 0}, {8 * kColumns, 16 * kRows}, bg_color_);
  for (int row = 0; row < kRows; ++row) {
    WriteString(*writer_, Vector2D<int>{0, 16 * row}, buffer_[row], fg_color_);
  }
}
// #@@range_end(refresh)

Console *console;

namespace {
  char console_buf[sizeof(Console)];
} // namespace

void InitializeConsole() {
  console = new(console_buf) Console{
    kDesktopFGColor, kDesktopBGColor
  };
  console->SetWriter(screen_writer);
}
