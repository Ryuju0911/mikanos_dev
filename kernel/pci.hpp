#pragma once

#include <array>
#include <cstdint>

#include "error.hpp"

namespace pci {
  /** @brief IO port address for CONFIG_ADDRESS register */
  const uint16_t kConfigAddress = 0x0cf8;

  /** @brief IO port address for CONFIG_DATA register */
  const uint16_t kConfigData = 0x0cfc;

  /** @brief Classcode of PCI device */
  struct ClassCode {
    uint8_t base, sub, interface;

    bool Match(uint8_t b) { return b == base; }
    bool Match(uint8_t b, uint8_t s) { return Match(b) && s == sub; }
    bool Match(uint8_t b, uint8_t s, uint8_t i) {
      return Match(b, s) && i == interface;
    }
  };

  /** @brief Stores the basic data needed to operate a PCI device. */
  struct Device {
    uint8_t bus, device, function, header_type;
    ClassCode class_code;
  };

  /** @brief Reads Vendor ID register (for all the header types). */
  uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);
  uint16_t ReadVendorId(const Device& dev);

  /** @brief Reads Device ID register (for all the header types). */
  uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief Reads Header Type register (for all the header types). */
  uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief Reads Class Code register (for all the header types).
   * The structure of returned 32-bit interger:
   *  - 31:24 : Base Class
   *  - 16:23 : Sub Class
   *  - 15:8  : Interface
   *  - 7:0   : Revision ID
   */ 
  ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief Reads Bus Numbers register (for header type 1)
   * The structure of returned 32-bit integer:
   *  - 23:16 : Subordinate bus number
   *  - 15:8  : Secondary bus number
   *  - 7:0   : Revision number
   */
  uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief Reads 32-bit register of the specified PCI device. */
  uint32_t ReadConfReg(const Device &dev, uint8_t reg_addr);

  void WriteConfReg(const Device &dev, uint8_t reg_addr, uint32_t value);

  /** @brief Returns true if the device has only a single function. */
  bool IsSingleFunctionDevice(uint8_t header_type);

  /** @brief PCI devices found by ScanAllBus() */
  inline std::array<Device, 32> devices;

  /** @brief The number of valid devices */
  inline int num_device;

  /** @brief Explore all the PCI devices, and store them in 'devices'. */
  Error ScanAllBus();

  constexpr uint8_t CalcBarAddress(unsigned int bar_index) {
    return 0x10 + 4 * bar_index;
  }

  WithError<uint64_t> ReadBar(Device &device, unsigned int bar_index);

  /** @brief Common header of PCI capability register */
  union CapabilityHeader {
    uint32_t data;
    struct {
      uint32_t cap_id : 8;
      uint32_t next_ptr : 8;
      uint32_t cap : 16;
    } __attribute__((packed)) bits;
  } __attribute__((packed));

  const uint8_t kCapabilityMSI = 0x05;
  const uint8_t kCapabilityMSIX = 0x11;

  /** @brief Reads the specified capability header of the specified PCI device.
   * 
   * @param dev PCI device of which this function reads capability header
   * @param addr Configuration space address of capability register
   */
  CapabilityHeader ReadCapabilityHeader(const Device &dev, uint8_t addr);

  /** @brief MSI capability structure
   * 
   * The MSI capability structure has many variants,
   * with or without 64-bit support.
   * This structure defines members for the largest variants to
   * accommodate each variant.
   */
  struct MSICapability {
    union {
      uint32_t data;
      struct {
        uint32_t cap_id : 8;
        uint32_t next_ptr : 8;
        uint32_t msi_enable : 1;
        uint32_t multi_msg_capable : 3;
        uint32_t multi_msg_enable : 3;
        uint32_t addr_64_capable : 1;
        uint32_t per_vector_mask_capable : 1;
        uint32_t : 7;
      } __attribute__((packed)) bits;
    } __attribute__((packed)) header;

    uint32_t msg_addr;
    uint32_t msg_upper_addr;
    uint32_t msg_data;
    uint32_t mask_bits;
    uint32_t pending_bits;
  } __attribute__((packed));

  /** @brief Sets MSI/MSI-X interruption.
   * 
   * @param dev target PCI device
   * @param msg_addr Address to which message is written
   *                 when an interrupt occurs
   * @param msg_data Value of message to be written when an interrupt occurs
   * @param num_vector_exponent Number of vectors to allocate (specify n in 2^n)
   */
  Error ConfigureMSI(const Device &dev, uint32_t msg_addr, uint32_t msg_data,
                     unsigned int num_vector_exponent);

  enum class MSITriggerMode {
    kEdge = 0,
    kLevel = 1
  };

  enum class MSIDeliveryMode {
    kFixed          = 0b000,
    kLowestPriority = 0b001,
    kSMI            = 0b010,
    kNMI            = 0b100,
    kINIT           = 0b101,
    kExtINT         = 0b111,
  };

  Error ConfigureMSIFixedDestination(
    const Device &dev, uint8_t apic_id,
    MSITriggerMode trigger_mode, MSIDeliveryMode delivery_mode,
    uint8_t vector, unsigned int num_vector_exponent);
}

void InitializePCI();
