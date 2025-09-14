#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include <GLFW/glfw3.h>
#include <imgui.h>

template <class... Ts>
struct Overloads : Ts...
{
    using Ts::operator()...;
};

// stroke state machine
struct StrokeIdle
{
};
struct StrokeActive
{
    ImVec2 start_pos;
};
using StrokeState = std::variant<StrokeIdle, StrokeActive>;

class Whiteboard28
{
public:
    Whiteboard28();
    ~Whiteboard28();

    static constexpr size_t WIDTH = 28;
    static constexpr size_t HEIGHT = 28;
    static constexpr size_t PIXEL_COUNT = WIDTH * HEIGHT;
    static constexpr ImVec2 SIZE{WIDTH, HEIGHT};

    using Buffer = std::array<uint8_t, PIXEL_COUNT>;

    static ImVec2 getLogicalPos();

    void render(ImVec2 img_size);
    void applyBrush(ImVec2 pos);

    // Get the current pixel buffer for external processing
    const Buffer &getPixelBuffer() const { return _pixels; }

    void initGL();
    void updateGL();
    void shutdownGL();

private:
    static constexpr size_t KERNEL_SIZE = 5;
    static constexpr size_t KERNEL_RADIUS = KERNEL_SIZE / 2;
    static constexpr std::array<std::array<float, KERNEL_SIZE>, KERNEL_SIZE> GAUSSIAN_KERNEL{{
        {{0.005f, 0.028f, 0.050f, 0.028f, 0.005f}},
        {{0.028f, 0.158f, 0.281f, 0.158f, 0.028f}},
        {{0.050f, 0.281f, 0.500f, 0.281f, 0.050f}},
        {{0.028f, 0.158f, 0.281f, 0.158f, 0.028f}},
        {{0.005f, 0.028f, 0.050f, 0.028f, 0.005f}},
    }};

    Buffer _pixels{};

    StrokeState _stroke_state = StrokeIdle{};

    GLuint _gl_texture = 0;
};

// communication interface
class IWhiteboardComm
{
public:
    virtual ~IWhiteboardComm() = default;

    // Send a 28x28 pixel buffer for inference (type-safe)
    virtual void send(const Whiteboard28::Buffer &buffer) = 0;

    // Poll for latest prediction. Returns true if new prediction available.
    // digit will be set to 0-9 if prediction is available.
    virtual std::optional<uint8_t> recv() = 0;
};
