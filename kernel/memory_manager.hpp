#pragma once

#include <array>
#include <cstddef>

#include "error.hpp"
#include "memory_map.hpp"

// #@@range_begin(frame_id)
namespace {
  constexpr unsigned long long operator""_KiB(unsigned long long kib) {
    return kib * 1024;
  }

  constexpr unsigned long long operator""_MiB(unsigned long long mib) {
    return mib * 1024_KiB;
  }

  constexpr unsigned long long operator""_GiB(unsigned long long gib) {
    return gib * 1024_MiB;
  }
} //namespace

static const auto kBytesPerFrame{4_KiB};

/** @brief size of one physical memory frame (bytes) */
class FrameID {
  public:
    explicit FrameID(size_t id) : id_{id} {}
    size_t ID() const { return id_; }
    void *Frame() const { return reinterpret_cast<void*>(id_ * kBytesPerFrame); }
  private:
    size_t id_;
};

static const FrameID kNullFrame(std::numeric_limits<size_t>::max());
// #@@range_end(frame_id)

// #@@range_begin(bitmap_memory_manager)
/** @brief Per-frame memory management class using a bitmap array.
 * 
 * Manages free frames using a bitmap array.
 * Each bit of alloc_map corresponds to a frame, 0 means ree, 1 means in use.
 * The physical address of m-th bit of alloc_map[n] can be got by the
 * following formula.
 *  kFrameBytes * (n * kBitsPerMapLine + m)
 */
class BitmapMemoryManager {
  public:
    /** @brief Maximum amount of physical memory
     * this memory management class can handle (bytes)
     */ 
    static const auto kMaxPhysicalMemoryBytes{128_GiB};

    /** @brief The number of frames required to handle physical memory
     * up to kMaxPhysicalMemoryBytes
     */
    static const auto kFrameCount{kMaxPhysicalMemoryBytes / kBytesPerFrame};

    /** @brief Element type of bitmap array */
    using MapLineType = unsigned long;

    /** @brief The number of bits in a single element of the bitmap array
     * == the number of frames
     */
    static const size_t kBitsPerMapLine{8 * sizeof(MapLineType)};

    BitmapMemoryManager();

    WithError<FrameID> Allocate(size_t num_frames);
    Error Free(FrameID start_frame, size_t num_frames);
    void MarkAllocated(FrameID start_frame, size_t num_frames);

    /** @brief Sets the range of memory this memory manager class handles.
     * Since calling this method, 'Allocate' does memory allocation
     * within the range.
     */ 
    void SetMemoryRange(FrameID range_begin, FrameID range_end);
  
  private:
    std::array<MapLineType, kFrameCount / kBitsPerMapLine> alloc_map_;
    FrameID range_begin_;
    FrameID range_end_;

    bool GetBit(FrameID frame) const;
    void SetBit(FrameID frame, bool allocated);
};
// #@@range_end(bitmap_memory_manager)

extern BitmapMemoryManager *memory_manager;
void InitializeMemoryManager(const MemoryMap &memory_map);
