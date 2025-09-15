module;

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <variant>

#include <glad/gl.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

export module ebnn_host:canvas;

// Export utility templates
export template <class... Ts>
struct Overloads : Ts...
{
    using Ts::operator()...;
};

// Export stroke state machine types
export struct StrokeIdle
{
};

export struct StrokeActive
{
    ImVec2 start_pos;
};

export using StrokeState = std::variant<StrokeIdle, StrokeActive>;

// Export the main Canvas28 class
export class Canvas28
{
public:
    Canvas28()
    {
        _pixels.fill(0);
    }

    ~Canvas28()
    {
        shutdownGL();
    }

    static constexpr size_t WIDTH = 28;
    static constexpr size_t HEIGHT = 28;
    static constexpr size_t PIXEL_COUNT = WIDTH * HEIGHT;
    static constexpr ImVec2 SIZE{WIDTH, HEIGHT};

    using Buffer = std::array<uint8_t, PIXEL_COUNT>;

    static ImVec2 getLogicalPos()
    {
        return Canvas28::SIZE * (ImGui::GetIO().MousePos - ImGui::GetItemRectMin()) / ImGui::GetItemRectSize();
    }

    void render(ImVec2 img_size)
    {
        if (_gl_texture == 0)
            return;

        ImGui::Image((ImTextureID)(intptr_t)_gl_texture, img_size);

        auto visitor = Overloads{
            [this](StrokeIdle) -> StrokeState
            {
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                {
                    ImVec2 logical_pos = Canvas28::getLogicalPos();
                    applyBrush(logical_pos);
                    return StrokeActive{logical_pos};
                }
                return StrokeIdle{};
            },

            [this](StrokeActive state) -> StrokeState
            {
                ImVec2 logical_pos = Canvas28::getLogicalPos();
                ImVec2 offset = logical_pos - state.start_pos;
                bool do_brush = ImGui::IsItemHovered() && std::hypot(offset.x, offset.y) > 0.03f;

                if (do_brush)
                {
                    applyBrush(logical_pos);
                }
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    return StrokeIdle{};
                }
                if (do_brush)
                {
                    return StrokeActive{logical_pos};
                }
                return state;
            }};

        _stroke_state = std::visit(visitor, _stroke_state);
        updateGL();
    }

    void applyBrush(ImVec2 pos)
    {
        size_t center_x = static_cast<size_t>(pos.x);
        size_t center_y = static_cast<size_t>(pos.y);

        for (size_t ky = 0; ky < KERNEL_SIZE; ++ky)
        {
            for (size_t kx = 0; kx < KERNEL_SIZE; ++kx)
            {
                size_t ix = center_x + (kx - KERNEL_RADIUS);
                size_t iy = center_y + (ky - KERNEL_RADIUS);

                if (ix >= WIDTH || iy >= HEIGHT)
                    continue;

                size_t idx = iy * WIDTH + ix;
                float w = GAUSSIAN_KERNEL[ky][kx];

                uint16_t v = static_cast<uint16_t>(_pixels[idx]);
                v += static_cast<uint16_t>((255ui16 - v) * w);
                _pixels[idx] = static_cast<uint8_t>(std::min(v, 255ui16));
            }
        }
    }

    void clear()
    {
        _pixels.fill(0);
        updateGL();
    }

    const Buffer &getPixelBuffer() const { return _pixels; }

    void initGL()
    {
        glGenTextures(1, &_gl_texture);
        glBindTexture(GL_TEXTURE_2D, _gl_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, WIDTH, HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, _pixels.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void updateGL()
    {
        if (_gl_texture == 0)
            return;

        glBindTexture(GL_TEXTURE_2D, _gl_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RED, GL_UNSIGNED_BYTE, _pixels.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void shutdownGL()
    {
        if (_gl_texture != 0)
        {
            glDeleteTextures(1, &_gl_texture);
            _gl_texture = 0;
        }
    }

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