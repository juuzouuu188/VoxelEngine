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

    if (ImGui::Button("Delete All Chunks")) {
        chunkManager->deleteAllChunks();
    }

    ImGui::End();

    if (regenerate) {
        regenerate = false;

        // Delete old chunks first
        chunkManager->deleteAllChunks();

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

void UIManager::renderStats(MasterRenderer* masterRenderer) {
    ImGui::Begin("Debug");
    size_t count = masterRenderer->totalVertices();  // make sure the function name matches
    ImGui::Text("Total vertices: %zu", count);      // use %zu for size_t


    // Wireframe toggle
    if (ImGui::Checkbox("Wireframe Mode", &wireframeMode)) {
        if (wireframeMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // enable wireframe
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // back to normal
    }

    // Greedy Mesh toggle
    bool greedy = masterRenderer->getGreedyMesh();
    if (ImGui::Checkbox("Use Greedy Mesh", &greedy)) {
        masterRenderer->setGreedyMesh(greedy); // apply the change
    }

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