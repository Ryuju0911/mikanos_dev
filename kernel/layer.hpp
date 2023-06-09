#pragma once

#include <map>
#include <memory>
#include <vector>


#include "error.hpp"
#include "graphics.hpp"
#include "message.hpp"
#include "window.hpp"

class Layer {
  public:
    Layer(unsigned int id = 0);
    unsigned int ID() const;

    Layer &SetWindow(const std::shared_ptr<Window> &window);
    std::shared_ptr<Window> GetWindow() const;
    Vector2D<int> GetPosition() const;

    Layer &SetDraggable(bool draggable);
    bool IsDraggable() const;

    

    /** @brief Updates the layer position to the specified absolute coordinates.
     * Doesn't redraws the contentes.
     */
    Layer &Move(Vector2D<int> pos);

    /** @brief Updates the layer position to the specified relative coordinates.
     * Doesn't redraws the contentes.
     */
    Layer &MoveRelative(Vector2D<int> pos_diff);

    /** @brief Draws the contents of the window currently set to writer. */
    void DrawTo(FrameBuffer &screen, const Rectangle<int> &area) const;

  private:
    unsigned int id_;
    Vector2D<int> pos_{};
    std::shared_ptr<Window> window_{};
    bool draggable_{false};
};

class LayerManager {
  public:
    void SetWriter(FrameBuffer *screen);
    Layer &NewLayer();
    void RemoveLayer(unsigned int id);

    /** @brief Draws a layer that is currently visible. */
    void Draw(const Rectangle<int> &area) const;

    /** @brief Redraws the drawing area of the window that is
     * set to the specified layer.
     */
    void Draw(unsigned int id) const;

    /** @brief Redraws the specified drawing area of the window
     * that is set to the specified layer.
     */
    void Draw(unsigned int id, Rectangle<int> area) const;

    void Move(unsigned int id, Vector2D<int> new_pos);
    void MoveRelative(unsigned int id, Vector2D<int> pos_diff);

    

    /** @brief Moves the layer to the specified height.
     * 
     * If new_height < 0, the layer is hidden.
     * If new_height >= 0, the layer is moved to the height.
     * If new_height >= the current number of the layers,
     * the layer becomes the topmost layer.
     */
    void UpDown(unsigned int id, int new_height);

    /** @brief Hides the specified layer. */
    void Hide(unsigned int id);

    /** @brief Finds the uppermost visible layer with a window
     * at the specified coordinates.
     */
    Layer *FindLayerByPosition(Vector2D<int> pos, unsigned int exclude_id) const;

    /** @brief Finds the layer with the specified id. */
    Layer *FindLayer(unsigned int id);

    int GetHeight(unsigned int id);

  private:
    FrameBuffer *screen_{nullptr};
    mutable FrameBuffer back_buffer_{};
    std::vector<std::unique_ptr<Layer>> layers_{};
    std::vector<Layer*> layer_stack_{};
    unsigned int latest_id_{0};
};

extern LayerManager *layer_manager;

class ActiveLayer {
  public:
    ActiveLayer(LayerManager &manager);
    void SetMouseLayer(unsigned int mouse_layer);
    void Activate(unsigned int layer_id);
    unsigned int GetActive() const { return active_layer_; }

  private:
    LayerManager &manager_;
    unsigned int active_layer_{0};
    unsigned int mouse_layer_{0};
};

extern ActiveLayer *active_layer;
extern std::map<unsigned int, uint64_t> *layer_task_map;

void InitializeLayer();
void ProcessLayerMessage(const Message &msg);

constexpr Message MakeLayerMessage(
    uint64_t task_id, unsigned int layer_id,
    LayerOperation op, const Rectangle<int> &area) {
  Message msg{Message::kLayer, task_id};
  msg.arg.layer.layer_id = layer_id;
  msg.arg.layer.op = op;
  msg.arg.layer.x = area.pos.x;
  msg.arg.layer.y = area.pos.y;
  msg.arg.layer.w = area.size.x;
  msg.arg.layer.h = area.size.y;
  return msg;
}

Error CloseLayer(unsigned int layer_id);
