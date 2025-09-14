#include <optional>

#include "ebnn_host/duck_comm.h"
#include "ebnn_host/whiteboard.h"

void DuckWhiteboardComm::send(const Whiteboard28::Buffer &pixels)
{
    _counter = (_counter + 1) % 10;
}

std::optional<uint8_t> DuckWhiteboardComm::recv()
{
    return std::make_optional(_counter);
}
