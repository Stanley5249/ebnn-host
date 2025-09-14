#pragma once

#include <chrono>
#include <memory>
#include <optional>

#include "ebnn_host/duck_comm.h"
#include "ebnn_host/whiteboard.h"

/**
 * @brief Main application class for the MNIST whiteboard
 *
 * Handles the complete application lifecycle including ImGui rendering,
 * whiteboard interaction, and communication with prediction systems.
 * Features a modern responsive layout with proper UI scaling.
 */
class WhiteboardApp
{
public:
    WhiteboardApp();
    ~WhiteboardApp();

    // Disable copy/move for simplicity
    WhiteboardApp(const WhiteboardApp &) = delete;
    WhiteboardApp &operator=(const WhiteboardApp &) = delete;
    WhiteboardApp(WhiteboardApp &&) = delete;
    WhiteboardApp &operator=(WhiteboardApp &&) = delete;

    /**
     * @brief Render one frame of the application
     *
     * Handles complete UI rendering including layout, interaction,
     * and communication polling. Call once per frame in main loop.
     */
    void renderFrame();

private:
    // Core components
    std::unique_ptr<Whiteboard28> _whiteboard;
    std::unique_ptr<IWhiteboardComm> _comm;

    // State management
    std::optional<uint8_t> _last_prediction; // Last predicted digit (0-9)
    std::chrono::steady_clock::time_point _last_send_time;
    std::chrono::milliseconds _send_rate_limit{200}; // 200ms rate limit

    // UI Layout functions - break down the large RenderFrame
    void renderMainLayout();
    void renderWhiteboardColumn();
    void renderPredictionColumn();

    // Communication and state
    void handleRateLimitedSending();
};
