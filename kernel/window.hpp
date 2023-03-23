#pragma once

#include <optional>
#include <string>
#include <vector>

#include "frame_buffer.hpp"
#include "frame_buffer_config.hpp"
#include "graphics.hpp"

/** @brief Represents a graphic display area. */ 
class Window {
  public:
    // #@@range_begin(windowwriter)
    class WindowWriter : public PixelWriter {
      public:
        WindowWriter(Window &window) : window_{window} {}

        /** @brief Draws the specified color at the specified position. */
        virtual void Write(Vector2D<int> pos, const PixelColor &c) override {
          window_.Write(pos, c);
        }

        virtual int Width() const override { return window_.Width(); }
        virtual int Height() const override { return window_.Height(); }
      
      private:
        Window &window_;
    };
    // #@@range_end(windowwriter)

    Window(int width, int height, PixelFormat shadow_format);
    virtual ~Window() = default;
    Window(const Window &rhs) = delete;
    Window &operator=(const Window &rhs) = delete;
    
    void SetTransparentColor(std::optional<PixelColor> c);

    /** @brief Draws the display area of this window to the given PixelWriter.
     * 
     * @param dst Target
     * @param position Drawing position relative to the upper left corner of dst
     * @param area Drawing area relative to the upper left corner of dst
     */ 
    void DrawTo(FrameBuffer& dst, Vector2D<int> pos,
                const Rectangle<int> &area);

    /** @brief Moves the rectangular area within the plane drawing area of
     * this window.
     * 
     * @param dst_pos origin of destination
     * @param src_pos origin of source rectangle
     * @param src_size size of source rectangle
     */
    void Move(Vector2D<int> dst_pos, const Rectangle<int> &src);

    void Write(Vector2D<int> pos, PixelColor c);
    const PixelColor &At(Vector2D<int> pos) const;

    virtual void Activate() {}
    virtual void Deactivate() {}

    // Getter functions
    WindowWriter *Writer();
    int Width() const;
    int Height() const;
    Vector2D<int> Size() const;

  private:
    int width_, height_;
    std::vector<std::vector<PixelColor>> data_{};
    WindowWriter writer_{*this};
    std::optional<PixelColor> transparent_color_{std::nullopt};
    FrameBuffer shadow_buffer_{};
};

class ToplevelWindow : public Window {
  public:
    static constexpr Vector2D<int> kTopLeftMargin{4, 24};
    static constexpr Vector2D<int> kBottomRightMargin{4, 4};
    static constexpr int kMarginX = kTopLeftMargin.x + kBottomRightMargin.x;
    static constexpr int kMarginY = kTopLeftMargin.y + kBottomRightMargin.y;

    class InnerAreaWriter : public PixelWriter {
      public:
        InnerAreaWriter(ToplevelWindow &window) : window_{window} {}

        virtual void Write(Vector2D<int> pos, const PixelColor &c) override {
          window_.Write(pos + kTopLeftMargin, c);
        }
        
        virtual int Width() const override {
          return window_.Width() - kTopLeftMargin.x - kBottomRightMargin.x;
        }

        virtual int Height() const override {
          return window_.Height() - kTopLeftMargin.y - kBottomRightMargin.y;
        }

      private:
        ToplevelWindow &window_;
    };

    ToplevelWindow(int width, int height, PixelFormat shadow_format,
                   const std::string &title);
    
    virtual void Activate() override;
    virtual void Deactivate() override;

    InnerAreaWriter *InnerWriter() { return &inner_writer_; }
    Vector2D<int> InnerSize() const;

  private:
    std::string title_;
    InnerAreaWriter inner_writer_{*this};
};

void DrawWindow(PixelWriter &writer, const char *title);
void DrawTextbox(PixelWriter &writer, Vector2D<int> pos, Vector2D<int> size);
void DrawTerminal(PixelWriter &writer, Vector2D<int> pos, Vector2D<int> size);
void DrawWindowTitle(PixelWriter &writer, const char *title, bool active);
