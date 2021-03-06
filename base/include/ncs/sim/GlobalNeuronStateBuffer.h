#pragma once
#include <ncs/sim/DataBuffer.h>
#include <ncs/sim/DeviceType.h>
#include <ncs/sim/Memory.h>

namespace ncs {

namespace sim {

template<DeviceType::Type MType>
class GlobalNeuronStateBuffer : public DataBuffer {
public:
  GlobalNeuronStateBuffer(size_t global_neuron_vector_size);
  bool init();
  Bit::Word* getFireBits();
  size_t getVectorSize() const;
  size_t getNumberOfWords() const;
  bool isValid() const;
  ~GlobalNeuronStateBuffer();
private:
  Bit::Word* fire_bits_;
  size_t global_neuron_vector_size_;
};

} // namespace sim

} // namespace ncs

#include <ncs/sim/GlobalNeuronStateBuffer.hpp>
