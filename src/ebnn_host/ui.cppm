module;

#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

export module ebnn_host:ui;

import :canvas;
import :inference;

// Export the main application class
export class EBNNHost
{
public:
    EBNNHost()
    {
        _canvas = std::make_unique<Canvas28>();
        _canvas->initGL();
        _last_send_time = std::chrono::steady_clock::now();
    }

    ~EBNNHost()
    {
        if (_canvas)
        {
            _canvas->shutdownGL();
            _canvas.reset();
        }
    }

    EBNNHost(const EBNNHost &) = delete;
    EBNNHost &operator=(const EBNNHost &) = delete;
    EBNNHost(EBNNHost &&) = delete;
    EBNNHost &operator=(EBNNHost &&) = delete;

    void renderFrame()
    {
        if (ImGui::Begin(
                "MNIST Whiteboard",
                nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
        {
            renderMainLayout();
        }
        ImGui::End();

        ImGui::ShowDemoWindow();
    }

    void setInference(std::unique_ptr<IInference> inference)
    {
        _inference = std::move(inference);
    }

private:
    std::unique_ptr<Canvas28> _canvas;
    std::unique_ptr<IInference> _inference;
    std::optional<uint8_t> _last_prediction;
    std::chrono::steady_clock::time_point _last_send_time;
    std::chrono::milliseconds _send_rate_limit{200};

    void renderMainLayout()
    {
        if (ImGui::BeginTable(
                "MainLayout",
                2,
                ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PadOuterX))
        {
            ImGui::TableSetupColumn("Whiteboard");
            ImGui::TableSetupColumn("Result");
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            renderCanvasColumn();

            ImGui::TableNextColumn();
            renderPredictionColumn();

            ImGui::EndTable();
        }
    }

    void renderCanvasColumn()
    {
        ImVec2 available_size = ImGui::GetContentRegionAvail();
        float canvas_size = std::min(available_size.y, available_size.x) * 0.9f;
        canvas_size = std::clamp(canvas_size, 256.0f, 512.0f);

        ImGui::BeginGroup();
        ImGui::TextUnformatted("Draw a digit (0-9) with your mouse");
        ImGui::Spacing();
        _canvas->render(ImVec2(canvas_size, canvas_size));
        ImGui::EndGroup();

        handleRateLimitedSending();
    }

    void renderPredictionColumn()
    {
        ImGui::TextUnformatted("Prediction");
        ImGui::Spacing();

        if (_inference)
        {
            auto prediction = _inference->recv();
            if (prediction.has_value())
            {
                _last_prediction = prediction;
            }
        }

        if (!_last_prediction.has_value())
        {
            ImGui::TextUnformatted("-");
        }
        else if (_last_prediction >= 0 && _last_prediction <= 9)
        {
            ImGui::Text("%d", *_last_prediction);
        }
        else
        {
            ImGui::TextUnformatted("Error");
        }

        ImGui::Spacing();

        if (ImGui::Button("Clear"))
        {
            _canvas->clear();
            _last_prediction.reset();
        }
    }

    void handleRateLimitedSending()
    {
        auto current_time = std::chrono::steady_clock::now();

        if (_inference && (current_time - _last_send_time) >= _send_rate_limit)
        {
            const auto &pixel_buffer = _canvas->getPixelBuffer();
            _inference->send(pixel_buffer);
            _last_send_time = current_time;
        }
    }
};