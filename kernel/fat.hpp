#pragma once

#include <cstddef>
#include <cstdint>

#include "error.hpp"
#include "file.hpp"

namespace fat {

struct BPB {
  uint8_t jump_boot[3];
  char oem_name[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sector_count;
  uint8_t num_fats;
  uint16_t root_entry_count;
  uint16_t total_sectors_16;
  uint8_t media;
  uint16_t fat_size_16;
  uint16_t sectors_per_track;
  uint16_t num_heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_32;
  uint32_t fat_size_32;
  uint16_t ext_flags;
  uint16_t fs_version;
  uint32_t root_cluster;
  uint16_t fs_info;
  uint16_t backup_boot_sector;
  uint8_t reserved[12];
  uint8_t drive_number;
  uint8_t reserved1;
  uint8_t boot_signature;
  uint32_t volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute__((packed));

enum class Attribute : uint8_t {
  kReadOnly  = 0x01,
  kHidden    = 0x02,
  kSystem    = 0x04,
  kVolumeID  = 0x08,
  kDirectory = 0x10,
  kArchive   = 0x20,
  kLongName  = 0x0f,
};

struct DirectoryEntry {
  unsigned char name[11];
  Attribute attr;
  uint8_t ntres;
  uint8_t create_time_tenth;
  uint16_t create_time;
  uint16_t create_date;
  uint16_t last_access_date;
  uint16_t first_cluster_high;
  uint16_t write_time;
  uint16_t write_date;
  uint16_t first_cluster_low;
  uint32_t file_size;

  uint32_t FirstCluster() const {
    return first_cluster_low |
      (static_cast<uint32_t>(first_cluster_high) << 16);
  }
} __attribute__((packed));

extern BPB *boot_volume_image;
extern unsigned long bytes_per_cluster;
void Initialize(void *volume_image);

/** @brief Returns the memory address where the beginning sector of
 * the specified cluster is put.
 * 
 * @param cluster Claster number (starting at 2)
 * @return The memory address where the beginning sector of
 * the specified cluster is put
 */
uintptr_t GetClusterAddr(unsigned long cluster);

template <class T>
T *GetSectorByCluster(unsigned long cluster) {
  return reinterpret_cast<T*>(GetClusterAddr(cluster));
}

/** @brief Retrives the short name of the directory entry, and separates it
 * into the base name and extension.
 * The short name is stripped of padded white space (0x20) and null-terminated.
 * 
 * @param entry The directory entry from which the short name is retrieved.
 * @param base Pointer to an array where the base name will be stored.
 * The size of array must be at least 9 bytes.
 * @param ext Pointer to an array where extension will be stored.
 * The size of array must be at least 4 bytes.
 */
void ReadName(const DirectoryEntry &entry, char *base, char *ext);

/** @brief Copies the short name of the directory entry to
 * the destination buffer. If the extension of the short name is empty,
 * the funtion copies "<base>", otherwise it copies "<base>.<ext>".
 * 
 * @param entry The directory entry from which the file name is retrieved.
 * @param dest Pointer to the destination buffer.
 */
void FormatName(const DirectoryEntry& entry, char* dest);

static const unsigned long kEndOfClusterchain = 0x0ffffffflu;

/** @brief Returns the next cluster number for the specified cluster.
 * 
 * @param cluster Cluster number
 * @return Next cluster number (or kEndOfClusterChain if none)
 */
unsigned long NextCluster(unsigned long cluster);

/** @brief Finds a file from the specified directory.
 * 
 * @param path File name in 8 + 3 format (case insensitive)
 * @param directory_cluster Starting cluster of the directory
 * (if omitted, search from the root directory)
 * @return Pair of an entry for a file or a directory,
 * and a flag representing a post slash.
 * nullptr, if file or directory is not found.
 * true, if the entry has a slash.
 * If the entry in the middle of the path is a file, the search is abandoned,
 * and returns the entry and true, i
 */
std::pair<DirectoryEntry*, bool>
FindFile(const char* path, unsigned long directory_cluster = 0);

bool NameIsEqual(const DirectoryEntry &entry, const char *name);

/** @brief Copies the contents of the specified file into the buffer.
 * 
 * @param buf Where file contentsare stored
 * @param len Buffer size (unit of byte)
 * @param entry Directory entry representing file
 * @return The number of loaded bytes
 */
size_t LoadFile(void *buf, size_t len, const DirectoryEntry &entry);

bool IsEndOfClusterchain(unsigned long cluster);

uint32_t *GetFAT();

/** @brief 
 * 
 * @param eoc_cluster
 * @param n
 * @return
 */
unsigned long ExtendCluster(unsigned long eoc_cluster, size_t n);

/** @brief Returns am empty entry in the specified directory.
 * If the directory is full, the function extends a cluster and allocate
 * an empty entry,
 * 
 * @param dir_cluster The directory where the function looks for an empty entry
 * @return Empty entry
 */
DirectoryEntry *AllocateEntry(unsigned long dir_cluster);

/** @brief Sets the short file name to the directory entry.
 * 
 * @param entry The directory entry to which the function sets the file name
 * @param name The file name consisting of the base name and extension
 * joined by a dot
 */
void SetFileName(DirectoryEntry &entry, const char *name);

/** @brief Creates a file entry in the specified path.
 * 
 * @param path File path
 * @return Newly created file entry
 */
WithError<DirectoryEntry*> CreateFile(const char *path);

/** @brief Makes a chain consisting of the specified number of empty clusters.
 * 
 * @param n The number of cluster
 * @return The beginning cluster number of the chain
 */
unsigned long AllocateClusterChain(size_t n);

class FileDescriptor : public ::FileDescriptor {
  public:
    explicit FileDescriptor(DirectoryEntry &fat_entry);
    size_t Read(void *buf, size_t len) override;
    size_t Write(const void *buf, size_t len) override;
    size_t Size() const override { return fat_entry_.file_size; }
    size_t Load(void *buf, size_t len, size_t offset) override;

  private:
    DirectoryEntry &fat_entry_;
    size_t rd_off_ = 0;
    unsigned long rd_cluster_ = 0;
    size_t rd_cluster_off_ = 0;
    size_t wr_off_ = 0;
    unsigned long wr_cluster_ = 0;
    size_t wr_cluster_off_ = 0;
};

} // namespace fat
