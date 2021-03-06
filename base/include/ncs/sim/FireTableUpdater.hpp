namespace ncs {

namespace sim {

template<DeviceType::Type MType>
FireTableUpdater<MType>::FireTableUpdater()
  : fire_table_(nullptr),
    global_presynaptic_neuron_ids_(nullptr),
    synaptic_delays_(nullptr),
    subscription_(nullptr) {
}

template<DeviceType::Type MType>
bool FireTableUpdater<MType>::
init(FireTable<MType>* table,
     SpecificPublisher<GlobalNeuronStateBuffer<MType>>* publisher,
     const std::vector<Synapse*> synapse_vector) {
  fire_table_ = table;
  std::vector<unsigned int> global_presynaptic_neuron_ids;
  std::vector<unsigned int> synaptic_delays;
  for (auto synapse : synapse_vector) {
    if (nullptr == synapse) {
      const unsigned int null_value = std::numeric_limits<unsigned int>::max();
      global_presynaptic_neuron_ids.push_back(null_value);
      synaptic_delays.push_back(null_value);
    } else {
      unsigned int global_id = synapse->presynaptic_neuron->id.global;
      global_presynaptic_neuron_ids.push_back(global_id);
      synaptic_delays.push_back(synapse->delay);
    }
  }
  auto num_words_per_vector = fire_table_->getWordsPerVector();
  for (size_t i = 0; i < fire_table_->getNumberOfRows(); ++i) {
    auto blank = new SynapticFireVectorBuffer<MType>(num_words_per_vector);
    if (!blank->init()) {
      std::cerr << "Failed to initialize SynapticFireVectorBuffer" <<
        std::endl;
      delete blank;
      return false;
    }
    this->addBlank(blank);
  }
  bool result = true;
  if (!global_presynaptic_neuron_ids.empty()) {
    result &= Memory<MType>::malloc(global_presynaptic_neuron_ids_,
                                    global_presynaptic_neuron_ids.size());
    result &= Memory<MType>::malloc(synaptic_delays_, synaptic_delays.size());
  }
  if (!result) {
    std::cerr << "Failed to allocate synaptic data arrays." << std::endl;
    return false;
  }
  if (!global_presynaptic_neuron_ids.empty()) {
    result &= 
      mem::copy<MType, DeviceType::CPU>(global_presynaptic_neuron_ids_,
                                        global_presynaptic_neuron_ids.data(),
                                        global_presynaptic_neuron_ids.size());
    result &=
      mem::copy<MType, DeviceType::CPU>(synaptic_delays_,
                                        synaptic_delays.data(),
                                        synaptic_delays.size());
  }
  if (!result) {
    std::cerr << "Failed to copy synaptic data arrays." << std::endl;
    return false;
  }
  device_synaptic_vector_size_ = synapse_vector.size();
  subscription_ = publisher->subscribe();
  return nullptr != subscription_;
}

template<DeviceType::Type MType>
bool FireTableUpdater<MType>::start(std::function<bool()> thread_init,
                                    std::function<bool()> thread_destroy) {
  auto thread_function = [this, thread_init, thread_destroy]() {
    thread_init();
    unsigned int min_delay = fire_table_->getMinDelay();
    unsigned int max_delay = fire_table_->getMaxDelay();
    for (unsigned int i = 0; i < max_delay; ++i) {
      fire_table_->lockRow(i);
      if (!Memory<MType>::zero(fire_table_->getRow(i), 
                               fire_table_->getWordsPerVector())) {
        std::cerr << "Failed to zero FireTable row " << i << std::endl;
      }
    }
    for (unsigned int i = 0; i < min_delay; ++i) {
      auto blank = this->getBlank();
      blank->setData(fire_table_->getRow(i));
      blank->simulation_step = i;
      auto prerelease_function = [this, i]() {
        fire_table_->releaseRow(i);
      };
      blank->setPrereleaseFunction(prerelease_function);
      this->publish(blank);
    }
    unsigned int step = 0;
    while (true) {
      auto neuron_fire_buffer = subscription_->pull();
      if (nullptr == neuron_fire_buffer) {
        break;
      }
      unsigned int max_row = step + max_delay;
      fire_table_->lockRow(max_row);
      if (!Memory<MType>::zero(fire_table_->getRow(max_row),
                               fire_table_->getWordsPerVector())) {
        std::cerr << "Failed to zero FireTable row " << max_row << 
          std::endl;
      }
      if (!this->update_(neuron_fire_buffer, step)) {
        std::cerr << "Failed to update FireTable on step " << step <<
          std::endl;
      }
      unsigned int publishable_row = step + min_delay;
      auto blank = this->getBlank();
      blank->setData(fire_table_->getRow(publishable_row));
      blank->simulation_step = publishable_row;
      auto prerelease_function = [this, publishable_row]() {
        fire_table_->releaseRow(publishable_row);
      };
      blank->setPrereleaseFunction(prerelease_function);
      this->publish(blank);
      neuron_fire_buffer->release();
      ++step;
    }
    thread_destroy();
  };
  thread_ = std::thread(thread_function);
  return true;
}

template<DeviceType::Type MType>
FireTableUpdater<MType>::~FireTableUpdater() {
  if (thread_.joinable()) {
    thread_.join();
  }
  if (global_presynaptic_neuron_ids_) {
    Memory<MType>::free(global_presynaptic_neuron_ids_);
  }
  if (synaptic_delays_) {
    Memory<MType>::free(synaptic_delays_);
  }
  if (subscription_) {
    delete subscription_;
  }
}

} // namespace sim

} // namespace ncs
