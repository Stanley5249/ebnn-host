module;

#include <cstdint>
#include <optional>

export module ebnn_host:inference;

import :canvas;

// Export the inference interface
export class IInference {
public:
  virtual ~IInference() = default;
  virtual void send(const Canvas28::Buffer &buffer) = 0;
  virtual std::optional<uint8_t> recv() = 0;
};

// Export the dummy inference implementation
export class DummyInference : public IInference {
public:
  void send(const Canvas28::Buffer &buffer) override {
    _counter = (_counter + 1) % 10;
  }

  std::optional<uint8_t> recv() override {
    return std::make_optional(_counter);
  }

private:
  uint8_t _counter = 0;
};