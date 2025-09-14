
#include <algorithm>
#include <optional>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "ebnn_host/whiteboard_app.h"

WhiteboardApp::WhiteboardApp()

{
    _whiteboard = std::make_unique<Whiteboard28>();
    _comm = std::make_unique<DuckWhiteboardComm>();

    // Initialize OpenGL resources for the whiteboard
    _whiteboard->initGL();

    _last_send_time = std::chrono::steady_clock::now();
}

WhiteboardApp::~WhiteboardApp()
{
    // Clean up communication first
    if (_comm)
    {
        _comm.reset();
    }

    // Clean up whiteboard and its GL resources
    if (_whiteboard)
    {
        _whiteboard->shutdownGL();
        _whiteboard.reset();
    }
}

void WhiteboardApp::renderFrame()
{
    if (ImGui::Begin(
            "MNIST Whiteboard",
            nullptr,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
    {
        renderMainLayout();
    }
    ImGui::End();
}

void WhiteboardApp::renderMainLayout()
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
        renderWhiteboardColumn();

        ImGui::TableNextColumn();
        renderPredictionColumn();

        ImGui::EndTable();
    }

    ImGui::ShowDemoWindow();
}

void WhiteboardApp::renderWhiteboardColumn()
{
    ImVec2 available_size = ImGui::GetContentRegionAvail();

    float whiteboard_size = std::min(available_size.y, available_size.x) * 0.9f;

    whiteboard_size = std::clamp(whiteboard_size, 256.0f, 512.0f);

    ImGui::BeginGroup();

    ImGui::TextUnformatted("Draw a digit (0-9) with your mouse");
    ImGui::Spacing();

    _whiteboard->render(ImVec2(whiteboard_size, whiteboard_size));

    ImGui::EndGroup();

    handleRateLimitedSending();
}

void WhiteboardApp::renderPredictionColumn()
{

    ImGui::TextUnformatted("Prediction");
    ImGui::Spacing();

    // Poll for new predictions
    if (_comm)
    {
        auto prediction = _comm->recv();
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
        _whiteboard->clear();
        _last_prediction.reset();
    }
}

void WhiteboardApp::handleRateLimitedSending()
{
    auto current_time = std::chrono::steady_clock::now();

    if (_comm && (current_time - _last_send_time) >= _send_rate_limit)
    {
        const auto &pixel_buffer = _whiteboard->getPixelBuffer();
        _comm->send(pixel_buffer);
        _last_send_time = current_time;
    }
}
