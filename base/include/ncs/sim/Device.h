#pragma once

#include <ncs/sim/DataBuffer.h>
#include <ncs/sim/DeviceDescription.h>
#include <ncs/sim/DeviceType.h>
#include <ncs/sim/FactoryMap.h>
#include <ncs/sim/FireTable.h>
#include <ncs/sim/NeuronSimulator.h>
#include <ncs/sim/NeuronSimulatorUpdater.h>
#include <ncs/sim/PluginDescription.h>
#include <ncs/sim/SynapseSimulator.h>
#include <ncs/sim/VectorExchanger.h>

namespace ncs {

namespace sim {

class DeviceBase {
public:
  virtual DeviceType::Type getDeviceType() const = 0;
  virtual int getNeuronTypeIndex(const std::string& type) const = 0;
  virtual bool initialize(DeviceDescription* description,
                          FactoryMap<NeuronSimulator>* neuron_plugins,
                          FactoryMap<SynapseSimulator>* synapse_plugins,
                          MachineVectorExchanger* machine_vector_exchanger,
                          size_t global_neuron_vector_size) = 0;
private:
};

template<DeviceType::Type MType>
class Device : public DeviceBase {
public:
  virtual DeviceType::Type getDeviceType() const;
  virtual int getNeuronTypeIndex(const std::string& type) const;
  virtual bool initialize(DeviceDescription* description,
                          FactoryMap<NeuronSimulator>* neuron_plugins,
                          FactoryMap<SynapseSimulator>* synapse_plugins,
                          MachineVectorExchanger* machine_vector_exchanger,
                          size_t global_neuron_vector_size);
private:
  bool initializeNeurons_(DeviceDescription* description,
                          FactoryMap<NeuronSimulator>* neuron_plugins);
  bool initializeNeuronSimulator_(NeuronSimulator<MType>* simulator,
                                  NeuronPluginDescription* description);
  bool initializeNeuronVoltages_();
  bool initializeNeuronUpdater_();

  bool initializeVectorExchangers_(MachineVectorExchanger* machine_exchanger,
                                   size_t global_neuron_vector_size);

  bool initializeSynapses_(DeviceDescription* description,
                           FactoryMap<SynapseSimulator>* synapse_plugins);
  bool initializeSynapseSimulator_(SynapseSimulator<MType>* simulator,
                                  SynapsePluginDescription* description);

  bool initializeFireTable_();

  std::map<std::string, int> neuron_type_map_;
  std::vector<NeuronSimulator<MType>*> neuron_simulators_;
  std::vector<size_t> neuron_device_id_offsets_;
  size_t neuron_device_vector_size_;
  NeuronSimulatorUpdater<MType>* neuron_simulator_updater_;

  DeviceVectorExtractor<MType>* fire_vector_extractor_;
  GlobalVectorInjector<MType>* global_vector_injector_;

  FireTable<MType>* fire_table_;
  unsigned int min_synaptic_delay_;
  unsigned int max_synaptic_delay_;

  std::vector<SynapseSimulator<MType>*> synapse_simulators_;
  std::vector<size_t> synapse_device_id_offsets_;
  size_t device_synaptic_vector_size_;
};

} // namespace sim

} // namespace ncs

#include <ncs/sim/Device.hpp>