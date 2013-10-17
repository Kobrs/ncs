#pragma once
#include <thread>

#include <ncs/sim/DataType.h>
#include <ncs/sim/ReportDataBuffer.h>
#include <ncs/sim/Signal.h>

namespace ncs {

namespace sim {

class PublisherExtractor : public SpecificPublisher<Signal> {
public:
  PublisherExtractor();
  bool init(size_t output_offset,
            DataType::Type datatype,
            const std::vector<unsigned int>& indices,
            const std::string pin_name,
            Publisher* source_publisher,
            SpecificPublisher<ReportDataBuffer>* destination_publisher);
  bool start();
  ~PublisherExtractor();
private:
  size_t output_offset_;
  DataType::Type datatype_;
  std::vector<unsigned int> indices_;
  Publisher* source_publisher_;
  Publisher::Subscription* source_subscription_;
  typename SpecificPublisher<ReportDataBuffer>::Subscription* 
    destination_subscription_;
  std::string pin_name_;
  std::thread thread_;
};

} // namespace sim

} // namespace ncs