#include "Engine/UIManager.h"


UIManager::UIManager(Window* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window->getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::debugChunkManager(ChunkManager* chunkManager) {
    static int radius = 7; // initial value
    static bool regenerate = false;

    ImGui::Begin("Chunk Performance Test");

    ImGui::Text("Chunk Debugging");
    ImGui::SliderInt("Radius", &radius, 1, 20); // lets you pick radius from 1 to 20
    if (ImGui::Button("Generate Chunks")) {
        regenerate = true;
    }

    ImGui::End();

    if (regenerate) {
        regenerate = false;
        for (int x = -radius; x < radius; ++x) {
            for (int z = -radius; z < radius; ++z) {
                chunkManager->GetOrCreateChunk(x, z);
            }
        }
    }
}

void UIManager::fpsCounter(float& deltaTime) {
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
    ImGui::End();
}

void UIManager::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}