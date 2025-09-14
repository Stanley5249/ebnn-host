#pragma once

#include "ebnn_host/whiteboard.h"

// Minimal comm: always returns prediction "5"
class DuckWhiteboardComm : public IWhiteboardComm
{
public:
    void send(const Whiteboard28::Buffer &buffer) override;
    std::optional<uint8_t> recv() override;

private:
    uint8_t _counter = 0;
};
