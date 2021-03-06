#pragma once
#include <ncs/sim/Bit.h>

namespace ncs {

namespace sim {

template<DeviceType::Type MType>
class FireTable {
public:
  FireTable(size_t device_synaptic_vector_size,
            unsigned int min_delay,
            unsigned int max_delay);
  bool init();
  Bit::Word* getTable();
  Bit::Word* getRow(unsigned int index);
  size_t getNumberOfRows() const;
  size_t getWordsPerVector() const;
  bool lockRow(unsigned int index);
  bool releaseRow(unsigned int index);
  unsigned int getMinDelay() const;
  unsigned int getMaxDelay() const;
  ~FireTable();
private:
  Bit::Word* table_;
  size_t device_synaptic_vector_size_;
  unsigned int min_delay_;
  unsigned int max_delay_;
  size_t num_rows_;
  std::mutex row_lock_;
  std::vector<bool> row_free_;
  std::condition_variable row_freed_;
};

} // namespace sim

} // namespace ncs

#include <ncs/sim/FireTable.hpp>
