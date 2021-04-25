#pragma once

/*
 * Created by Kami-Kaze on 4/11/2021.
 * Next Steps:
 *      - todo: infinite scrolling
 *          -! concerns:
 *              loss of quality when zooming in
 *          - 'connect' screen bounds in update
 *              > left of left is left of right
 *              > below bottom is below top
 *          - use post processing to render duplicates
 *              > render to texture with -1 <= x, y <= 1
 *              > re-render texture mapped to actual screen bounds
 *                  > use texture settings to auto-mirror as needed
 *      - todo: R3
 *          - use index buffer and new compute shader to determine render order
 *              > see note about improving shader performance below as well
 *              > re-calculate index buffer every time our camera moves
 *              > use vC * vF to project all fireflies onto view axis
 *              > sort according to dot product
 *          - in vertex shader
 *              > use fireflies[indexBuffer[myId]] instead of fireflies[myId]
 *              > to determine which firefly to render
 *
 *      - todo: visualize color functions in ui. Consider:
 *              - https://github.com/soulthreads/imgui-plot
 *              - https://github.com/epezent/implot
 *              - see not about improving shader performance below as well
 *
 *      - todo: improve shader performance again (probably after all the other steps)
 *          > use index buffer and count variable
 *            to determine the count of fireflies we need to render
 *            (don't even try to render fireflies that are 'off').
 *          > simplify equations if possible
 *              - eg.: we use x: f'(x) = 0; y = f(i / x)
 *                     and often f(i / x) can be expressed in an easy way
 *                     if we precalculate x (at compile time)
 *          > prevent duplicate calculations
 *              - eg.:           y = x^2(a-x^2)
 *                  <=> x = x^2; y = x(a-x);
 *          > generate textures
 *              > no need to calculate color maps for fireflies
 *                every time we render it.
 *              > instead: use compute shader to create color (brightness/alpha?) maps for
 *                render shaders when ever our color parameters are being changed
 *                and remove (most) calculations from our render pipeline
 *              - side note: if we have these textures, we can 'display' an example firefly in our ui
 *                           so the user can see the effect of the color parameters more easy.
 *                           (no need to zoom in on a single (isolated) firefly for testing anymore)
 *
 */

#include <glm/glm.hpp>

#include <core/application/application.h>
#include <core/events/mouse_events.h>
#include <core/events/key_events.h>

#include <core/renderer/shader_program/shader_program.h>
#include <core/renderer/buffer/buffer.h>
#include <core/renderer/camera/camera.h>
#include <core/renderer/camera/camera_controller.h>

using namespace Engine;

static constexpr float TWO_PI = glm::radians(360.f);

struct FireflyData {
    glm::vec2 position = glm::vec2(0.f);
    float _p3 = 0, _p4 = 0;                         // padding

    glm::vec3 color = glm::vec3(0.f);

    float size = 0;
    float phi = 0, frequency = 0;
    float nudgePhi = 0, nudgeFrequency = 0;
    int colorIndex;
    float _n0 = 0, _n1 = 0, _n2 = 0;                // padding again...
};

class FirefliesApplication : public Application {
private:
    // playback state
    bool paused = false, holding = false, justPaused = false;
    float tElapsed = 0;

    /*
     * compute shader cluster config
     */
    static constexpr int
            COMPUTE_CLUSTERS_X = 3,
            COMPUTE_CLUSTERS_Y = 3,
            COMPUTE_CLUSTERS_Z = 3,
            COMPUTE_CLUSTER_SIZE_BASE = 10,

    COMPUTE_CLUSTER_SIZE_X = COMPUTE_CLUSTER_SIZE_BASE,
            COMPUTE_CLUSTER_SIZE_Y = COMPUTE_CLUSTER_SIZE_BASE,
            COMPUTE_CLUSTER_SIZE_Z = COMPUTE_CLUSTER_SIZE_BASE,
            COMPUTE_CLUSTERS = COMPUTE_CLUSTERS_X * COMPUTE_CLUSTERS_Y * COMPUTE_CLUSTERS_Z,
            COMPUTE_CLUSTER_SIZE = COMPUTE_CLUSTER_SIZE_X * COMPUTE_CLUSTER_SIZE_Y * COMPUTE_CLUSTER_SIZE_Z,
            NUM_FIREFLIES = COMPUTE_CLUSTERS * COMPUTE_CLUSTER_SIZE;

    /*
     * base config
     */
    int numColors = 5;
    float fireflySize = .015f;                              // size of a firefly
    float fireflyMaxFrequency = 1.5f;                       // max frequency of a firefly

    /*
     * Currently loaded colors
     * and enable switches
     */
    int numColorsLoaded = numColors;         // number of colors (0 = continuous)
    bool zeroColors = numColorsLoaded == 0;
    int* enableColors = nullptr;

    /*
     * Simulation parameters:
     *  simulation speed: how fast the simulation runs
     *  epsilon: how much a firefly changes per cycle
     *  muVision: how fast a fireflies vision decays
     *  muColor: how less interesting different colors are
     *
     *  used like this:
     *      nudge_i =  ... calculate raw nudge with firefly i
     *      nudge_i *= epsilon * pow(distance_i, muVision) * pow(color_distance_i, muColor);
     */

    float simulationSpeed = .8f;
    Uniform<float> epsilon = Uniform<float>("epsilon", 1e2f);
    Uniform<float> muVision = Uniform<float>("muVision", 3e1f);
    Uniform<float> muColor = Uniform<float>("muColor", 5e0f);
    Uniform<float> uDPhi = Uniform<float>("dPhi", 0);

    /*
     * Attractor
     */
    Uniform<int> attractorEnable = Uniform<int>("attractorEnable", 0);
    Uniform<float> attractorRange = Uniform<float>("attractorRange", .2f);
    Uniform<float> attractorStrength = Uniform<float>("attractorStrength", 1.f);
    Uniform<glm::vec2> attractorPosition = Uniform<glm::vec2>("attractorPosition", glm::vec2(0.f));

    /*
     * Rendering
     */

    CameraController cameraController = CameraController(glm::vec2(1.f), .1f, "view");
    ShaderProgram* renderShader = nullptr;
    ShaderProgram* computeShader = nullptr;

    Buffer<FireflyData>* computeBuffer = nullptr;
    Buffer<int>* colorBuffer = nullptr;

    /*
     * Render parameters:
     *  blinkThreshold: how much a firefly has to want to be lit, before it lights up
     *  brightnessFalloff: how much a firefly's light dims in the center of the lit part
     */
    Uniform<float> blinkThreshold = Uniform<float>("blinkThreshold", .65);
    Uniform<float> brightnessFalloff = Uniform<float>("brightnessFalloff", 1.685);

    // center color
    Uniform<int> renderCenter = Uniform<int>("renderCenter", 1);
    Uniform<float> centerLightBase = Uniform<float>("centerLightBase", .5f);
    Uniform<float> centerLightMu = Uniform<float>("centerLightMu", 2.f);
    Uniform<float> centerDarkBase = Uniform<float>("centerDarkBase", 0.f);
    Uniform<float> centerDarkMu = Uniform<float>("centerDarkMu", 15.f);

    Uniform<int> renderGlow = Uniform<int>("renderGlow", 1);
    Uniform<float> glowLightBase = Uniform<float>("glowLightBase", .5f);
    Uniform<float> glowLightMu = Uniform<float>("glowLightMu", 1.25f);
    Uniform<float> glowDarkBase = Uniform<float>("glowDarkBase", .65f);
    Uniform<float> glowDarkMu = Uniform<float>("glowDarkMu", .5f);

public:
    void init() override;
    void shutdown() override;
    void onContextAttach(Context* context) override;
    void onContextDetach(Context* context) override;

protected:
    void setup(ApplicationProps& props) override;

    void update(float dt) override;
    void render(Context* context) override;
    void renderImGui() override;

    void onEvent(Event& e) override;
private:
    bool onMouseButtonPressEvent(MouseButtonPressEvent& e);
    bool onMouseButtonReleaseEvent(MouseButtonReleaseEvent& e);
    bool onKeyPressEvent(KeyPressEvent& e);

    void initFireflies();
};



