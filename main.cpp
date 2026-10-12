// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <ctime> // for clock

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

//Start of additions - Kyan
enum class stateScreen
{
    BootupInfo,
    loadingScreen,
    desktopHome
};
//image loader
bool loadImage(const char* filename, GLuint* tTexture, int* tWidth, int* tHeight)
{
    int width = 0;
    int height = 0;
    int channels = 0;


    unsigned char* imageData = stbi_load(filename, &width, &height, &channels, 4);
    if (imageData == nullptr)
    {
        return false;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData);
    *tTexture = texture;
    *tWidth = width;
    *tHeight = height;

    return true;
}
//End of additions - Kyan

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Select GL version + let the backend select a GLSL version
    const char* glsl_version = nullptr;
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + generally GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + generally GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    GLuint vaultBoyTexture = 0;
    int vaultBoyX = 0;
    int vaultBoyY = 0;
    bool loadVaultBoy = loadImage("assets/vaultboy/frame_000_delay-0.1s.png", &vaultBoyTexture, &vaultBoyX, &vaultBoyY);
    if (!loadVaultBoy)
    {
        printf("Failed to load image");
    }

    // Start of additions - Luis
    // Wallpaper
    GLuint wallpaperTexture = 0;
    int wallpaperWidth = 0;
    int wallpaperHeight = 0;
    bool loadWallpaper = loadImage("assets/wallpaper.jpg", &wallpaperTexture, &wallpaperWidth, &wallpaperHeight);
    if (!loadWallpaper)
    {
        printf("Failed to load wallpaper.jpg\n");
    }
    // End of additions - Luis
    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.

     //Start of additions - Kyan
    ImFont* falloutFont = io.Fonts->AddFontFromFileTTF("../../misc/fonts/Px437_Verite_8x16.ttf", 12.0f * main_scale);
    IM_ASSERT(falloutFont != nullptr);
    io.FontDefault = falloutFont;
    //End of additions - Kyan
    // 
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    //Start of additions - Kyan
    stateScreen currScreen = stateScreen::BootupInfo;
    double stateStartTime = glfwGetTime();
    std::vector<GLuint> vaultBoyFrames;
    int vaultBoyWidth = 0;
    int vaultBoyHeight = 0;
        for (int i = 0; i < 19; i++)
        {
            char path[128];
            sprintf_s(path, "assets/vaultboy/frame_%03d_delay-0.1s.png", i);
            GLuint texture = 0;
            int width = 0;
            int height = 0;

            if (loadImage(path, &texture, &width, &height))
            {
                vaultBoyFrames.push_back(texture);
                vaultBoyWidth = width;
                vaultBoyHeight = height;
            }
        }
    //End of additions - Kyan

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Start of additions - Kyan
        double timePassed = glfwGetTime() - stateStartTime;

        if (currScreen == stateScreen::BootupInfo)
        {
            //Positioning
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

            //Colors
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 1.0f, 0.35f, 1.0f));

            //Make background transparent for new background
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

            
            //Startup rules and text
            ImGui::Begin("BootupInfo", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

            //Background changes
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImU32 topColor = IM_COL32(0, 15, 1, 255);
            ImU32 midColor = IM_COL32(0, 35, 15, 255);
            ImU32 botColor = IM_COL32(0, 20, 1, 255);

            float midY = windowPos.y + (windowSize.y * 0.42f);

            drawList->AddRectFilledMultiColor(windowPos, ImVec2(windowPos.x + windowSize.x, midY), topColor, topColor, midColor, midColor);
            drawList->AddRectFilledMultiColor(ImVec2(windowPos.x, midY), ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), midColor, midColor, botColor, botColor);

            float edgeWidth = windowSize.x * 0.12f;
            drawList->AddRectFilledMultiColor(windowPos, ImVec2(windowPos.x + edgeWidth, windowPos.y + windowSize.y),
                IM_COL32(0, 0, 0, 110), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 110));
            drawList->AddRectFilledMultiColor(ImVec2(windowPos.x + windowSize.x - edgeWidth, windowPos.y), ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
                IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 110), IM_COL32(0, 0, 0, 110), IM_COL32(0, 0, 0, 0));

            //Text content and size adjusting and font
            ImGui::PushFont(falloutFont);
            float screenWidth = ImGui::GetIO().DisplaySize.x;
            float textScaling = screenWidth / 1280.0f;
            textScaling *= 1.6f;

            if (textScaling < 1.2f)
                textScaling = 1.2f;

            if (textScaling > 2.0f)
                textScaling = 2.0f;

            ImGui::SetWindowFontScale(textScaling);

            //Left side of screen
            ImGui::Text("VAULT-TEC OS (A CSOPESY PROJECT)");
            ImGui::Separator();

            if(timePassed >= 0.6f)
                ImGui::Text("-=Vault-Tec Megatrends BIOS v1.07=-");

            if (timePassed >= 1.0f)
                ImGui::Text("[Copyright (C) 2077 Vault-Tec Industries]");
                ImGui::Spacing();

            if (timePassed >= 1.4f)
                ImGui::Text("MEMORY TEST .............. 64000K OK");

            if (timePassed >= 1.8f)
                ImGui::Text("CPU ...................... ONLINE");

            if (timePassed >= 2.2f)
                ImGui::Text("DISPLAY ADAPTER .......... ONLINE");

            if (timePassed >= 2.6f)
                ImGui::Text("I/O CONTROLLER ........... ONLINE");

            if (timePassed >= 3.0f)
                ImGui::Text("PRIMARY STORAGE .......... ONLINE");
                ImGui::Spacing();

            //Right side of screen
            float rightPanelX = windowSize.x * 0.60f;
            float rightPanelY = 100.0f;

            if (timePassed >= 3.4f)
            {
                ImGui::SetCursorPos(ImVec2(rightPanelX, rightPanelY));
                ImGui::Text("[ SYSTEM STATUS ]");
            }

            if (timePassed >= 3.8f)
            {
                ImGui::SetCursorPos(ImVec2(rightPanelX, rightPanelY + 35.0f));
                ImGui::Text("BOOT MODE ........... NORMAL");
            }

            if (timePassed >= 4.2f)
            {
                ImGui::SetCursorPos(ImVec2(rightPanelX, rightPanelY + 65.0f));
                ImGui::Text("SECURITY ............ ACTIVE");
            }

            if (timePassed >= 4.6f)
            {
                ImGui::SetCursorPos(ImVec2(rightPanelX, rightPanelY + 95.0f));
                ImGui::Text("POWER ............... STABLE");
            }

            if (timePassed >= 5.0f)
            {
                ImGui::SetCursorPos(ImVec2(rightPanelX, rightPanelY + 125.0f));
                ImGui::Text("NETWORK ............. OFFLINE");
            }

            if (timePassed >= 5.4f)
            {
                ImGui::SetCursorPos(ImVec2(rightPanelX, rightPanelY + 155.0f));
                ImGui::Text("TERMINAL ID ......... VT-01");
            }

                

            // Bottom side of screen
            float botBarY = windowSize.y - 120.0f;
            drawList->AddLine(ImVec2(30.0f, botBarY), ImVec2(windowSize.x - 30.0f, botBarY), IM_COL32(70, 235, 105, 180), 2.0f);

            if (timePassed >= 6.2f)
            {
                ImGui::SetCursorPos(ImVec2(35.0f, botBarY + 20.0f));
                ImGui::Text("> SYSTEM DIAGNOSTICS COMPLETE");
            }
            

            if (timePassed >= 6.8f)
            {
                float initiationProgress = (float)((timePassed - 6.8f) / 2.0f);
                if(initiationProgress > 1.0f)
                {
                    initiationProgress = 1.0f;
                }
                int initiationPercentage = (int)(initiationProgress * 100.0f);
                ImGui::SetCursorPos(ImVec2(35.0f, botBarY + 55.0f));
                ImGui::Text("> INITIATING BOOTUP SEQUENCE...%d%%", initiationPercentage);
            }


            //CRT like lines for classic feel
            for (float y = windowPos.y; y < windowPos.y + windowSize.y; y += 3.0f)
            {
                drawList->AddLine(ImVec2(windowPos.x, y), ImVec2(windowPos.x + windowSize.x, y), IM_COL32(0, 0, 0, 170), 1.0f);
            }

            ImGui::PopFont();
            ImGui::End();


            ImGui::PopStyleColor(2);

            if (timePassed >= 8.9)
            {
                currScreen = stateScreen::loadingScreen;
                stateStartTime = glfwGetTime();
            }
        }

            else if (currScreen == stateScreen::loadingScreen)
            {
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
                ImGui::Begin("LoadingScreen", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

                //background
                ImVec2 windowPos = ImGui::GetWindowPos();
                ImVec2 windowSize = ImGui::GetWindowSize();

                ImDrawList* drawList = ImGui::GetWindowDrawList();

                ImU32 topColor = IM_COL32(0, 15, 1, 255);
                ImU32 midColor = IM_COL32(0, 35, 15, 255);
                ImU32 botColor = IM_COL32(0, 20, 1, 255);

                float midY = windowPos.y + (windowSize.y * 0.42f);

                drawList->AddRectFilledMultiColor(windowPos, ImVec2(windowPos.x + windowSize.x, midY), topColor, topColor, midColor, midColor);
                drawList->AddRectFilledMultiColor(ImVec2(windowPos.x, midY), ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), midColor, midColor, botColor, botColor);

                float edgeWidth = windowSize.x * 0.12f;
                drawList->AddRectFilledMultiColor(windowPos, ImVec2(windowPos.x + edgeWidth, windowPos.y + windowSize.y),
                    IM_COL32(0, 0, 0, 110), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 110));
                drawList->AddRectFilledMultiColor(ImVec2(windowPos.x + windowSize.x - edgeWidth, windowPos.y), ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
                    IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 110), IM_COL32(0, 0, 0, 110), IM_COL32(0, 0, 0, 0));


                //Text content and size adjusting and font
                ImGui::PushFont(falloutFont);
                float screenWidth = ImGui::GetIO().DisplaySize.x;
                float textScaling = screenWidth / 1280.0f;
                textScaling *= 1.6f;

                if (textScaling < 1.2f)
                    textScaling = 1.2f;

                if (textScaling > 2.0f)
                    textScaling = 2.0f;

                ImGui::SetWindowFontScale(textScaling);
               

                // loading gif, ascii art and placement of both
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 1.0f, 0.35f, 1.0f));
                float centerX = windowSize.x * 0.5f;

                const char* vaultAscii = R"(                ██╗   ██╗ █████╗ ██╗   ██╗██╗  ████████╗    ████████╗███████╗ ██████╗             ██████╗ ███████╗
                ██║   ██║██╔══██╗██║   ██║██║  ╚══██╔══╝    ╚══██╔══╝██╔════╝██╔════╝            ██╔═══██╗██╔════╝
                ██║   ██║███████║██║   ██║██║     ██║          ██║   █████╗  ██║       ██████╗   ██║   ██║███████╗
                ╚██╗ ██╔╝██╔══██║██║   ██║██║     ██║          ██║   ██╔══╝  ██║       ╚═════╝   ██║   ██║╚════██║
                 ╚████╔╝ ██║  ██║╚██████╔╝███████╗██║          ██║   ███████╗╚██████╗            ╚██████╔╝███████║
                  ╚═══╝  ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝          ╚═╝   ╚══════╝ ╚═════╝             ╚═════╝ ╚══════╝)";
                ImGui::SetWindowFontScale(1.0f);
                ImVec2 baseAsciiSize = ImGui::CalcTextSize(vaultAscii);

                float maxAsciiWidth = windowSize.x * 0.82f;
                float asciiScaling = maxAsciiWidth / baseAsciiSize.x;
                if (asciiScaling > 1.0f)
                    asciiScaling = 1.0f;
                if (asciiScaling < 0.40f)
                    asciiScaling = 0.40;
                ImGui::SetWindowFontScale(asciiScaling);
                ImVec2 asciiSize = ImGui::CalcTextSize(vaultAscii);
                float asciiX = centerX - (asciiSize.x * 0.58f);
                float asciiY = windowSize.y * 0.06f;

                ImGui::SetCursorPos(ImVec2(asciiX, asciiY));
                ImGui::TextUnformatted(vaultAscii);

                // vault boy gif
                float gifHeight = windowSize.y * 0.32f;
                float gifRatio = 1.0f;
                if (vaultBoyHeight > 0)
                {
                    gifRatio = (float)vaultBoyWidth / (float)vaultBoyHeight;

                }
                float gifWidth = gifHeight * gifRatio;
                float gifX = centerX - (gifWidth * 0.5f);
                float gifY = asciiY + asciiSize.y + 20.0f;

                if (!vaultBoyFrames.empty())
                {
                    int currFrame = (int)(timePassed / 0.1f) % vaultBoyFrames.size();
                    ImGui::SetCursorPos(ImVec2(gifX, gifY));
                    ImGui::Image((ImTextureID)(intptr_t)vaultBoyFrames[currFrame], ImVec2(gifWidth, gifHeight));
                }

                //Loading bar
                float actualProgress = (float)(timePassed / 4.0f);

                if (actualProgress > 1.0f)
                    actualProgress = 1.0f;


                float progressBarWidth = windowSize.x * 0.35f;
                float progressBarHeight = 18.0f;
                float progressBarX = centerX - (progressBarWidth * 0.5f);
                float progressBarY = gifY + gifHeight + 25.0f;

                drawList->AddRect(ImVec2(windowPos.x + progressBarX, windowPos.y + progressBarY), ImVec2(windowPos.x + progressBarX + progressBarWidth, windowPos.y + progressBarY + progressBarHeight),
                    IM_COL32(70, 235, 105, 220), 0.0f, 0, 2.0f);

                drawList->AddRectFilled(ImVec2(windowPos.x + progressBarX + 3.0f, windowPos.y + progressBarY + 3.0f), ImVec2(windowPos.x + progressBarX + 3.0f + ((progressBarWidth - 6.0f) * actualProgress),
                    windowPos.y + progressBarY + progressBarHeight - 3.0f), IM_COL32(70, 235, 105, 180));

                ImGui::SetWindowFontScale(textScaling);
                const char* loadingTxt;
                if (actualProgress < 0.25f)
                {
                    loadingTxt = "MOUNTING SYSTEM FILES...";
                }
                else if (actualProgress < 0.50f)
                {
                    loadingTxt = "INITIALIZING SYSTEM SERVICES...";
                }
                else if (actualProgress < 0.75f)
                {
                    loadingTxt = "LOADING INTERFACE MODULES...";
                }
                else if (actualProgress < 1.0f)
                {
                    loadingTxt = "PREPARING DESKTOP ENVIRONMENT...";
                }
                else
                {
                    loadingTxt = "SYSTEM IS NOW READY";
                }
                ImVec2 loadTxtSize = ImGui::CalcTextSize(loadingTxt);
                ImGui::SetCursorPos(ImVec2(centerX - (loadTxtSize.x * 0.5f), progressBarY + 30.0f));
                ImGui::TextUnformatted(loadingTxt);
                
                ImGui::PopStyleColor();
                ImGui::PopFont();
                ImGui::End();

                if (timePassed >= 4.5)
                {
                    currScreen = stateScreen::desktopHome;
                    stateStartTime = glfwGetTime();
                }
            }

            // Start of additions - Luis
            // Desktop Home Screen
            else if (currScreen == stateScreen::desktopHome)
            {
                ImGuiIO& io = ImGui::GetIO();

                // 1. Desktop Background & Wallpaper Window
                ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
                ImGui::SetNextWindowSize(io.DisplaySize);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

                ImGuiWindowFlags desktopFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoBringToFrontOnFocus;

                if (ImGui::Begin("VaultTec-OS", nullptr, desktopFlags))
                {
                    ImVec2 windowPos = ImGui::GetWindowPos();
                    ImVec2 windowSize = ImGui::GetWindowSize();
                    ImDrawList* drawList = ImGui::GetWindowDrawList();

                    // Wallpaper
                    if (wallpaperTexture != 0)
                    {
                        drawList->AddImage(
                            (ImTextureID)(intptr_t)wallpaperTexture,
                            windowPos,
                            ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y)
                        );
                    }
                    else
                    {
                        ImU32 topColor = IM_COL32(0, 15, 1, 255);
                        ImU32 midColor = IM_COL32(0, 35, 15, 255);
                        ImU32 botColor = IM_COL32(0, 20, 1, 255);
                        float midY = windowPos.y + (windowSize.y * 0.42f);

                        drawList->AddRectFilledMultiColor(windowPos, ImVec2(windowPos.x + windowSize.x, midY), topColor, topColor, midColor, midColor);
                        drawList->AddRectFilledMultiColor(
                            ImVec2(windowPos.x, midY),
                            ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
                            midColor, midColor, botColor, botColor
                        );
                    }

                    // Clock
                    time_t now = time(0);
                    tm* ltm = localtime(&now);
                    char timeStr[64];
                    if (ltm)
                        strftime(timeStr, sizeof(timeStr), "%A %b %d, %Y | %I:%M:%S %p", ltm);
                    else
                        snprintf(timeStr, sizeof(timeStr), "SYSTEM TIME ERROR");

                    ImGui::SetCursorPos(ImVec2(windowSize.x - 360.0f, 15.0f));
                    ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.35f, 1.0f), "[ %s ]", timeStr);
                }
                ImGui::End();
                ImGui::PopStyleVar(2);

                static bool showStartMenu = false;
                static bool showFileExplorer = false;
                static bool showTaskManager = false;

                // Taskbar 
                float taskbarHeight = 45.0f;
                ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - taskbarHeight));
                ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, taskbarHeight));

                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.08f, 0.02f, 0.95f));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.8f, 0.2f, 0.5f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

                ImGuiWindowFlags taskbarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

                if (ImGui::Begin("TaskbarPanel", nullptr, taskbarFlags))
                {
                    float powerBtnWidth = 70.0f;
                    float rPadding = 8.0f;
                    ImGui::SetCursorPosY(8.0f);

                    // Button 1: Start Menu
                    if (ImGui::Button("[VAULT-TEC]", ImVec2(110, 30))) {
                        showStartMenu = !showStartMenu;
                    }

                    ImGui::SameLine();

                    // Button 2: File Explorer
                    if (ImGui::Button("[EXPLORER]", ImVec2(100, 30))) {
                        showFileExplorer = !showFileExplorer;
                    }
                    ImGui::SameLine();

                    // Button 3: Task Manager
                    if (ImGui::Button("[TASK MANAGER]", ImVec2(130, 30)))
                    {
                        showTaskManager = !showTaskManager;
                    }

                    //Power Button
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - powerBtnWidth - rPadding);
                    if (ImGui::Button("[PWR]", ImVec2(powerBtnWidth, 30)))
                    {
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                    }

                }
                ImGui::End();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor(2);

                // Start Menu
                if (showStartMenu)
                {
                    ImGui::SetNextWindowPos(ImVec2(10.0f, io.DisplaySize.y - taskbarHeight - 210.0f));
                    ImGui::SetNextWindowSize(ImVec2(220.0f, 200.0f));

                    if (ImGui::Begin("Vault-Tec Menu", &showStartMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
                    {
                        ImGui::Text("ROBCO OS v1.00");
                        ImGui::Separator();
                        if (ImGui::MenuItem("> User Profile")) {}
                        if (ImGui::MenuItem("> System Settings")) {}
                    }
                    ImGui::End();
                }

                // File Explorer 
                if (showFileExplorer)
                {
                    ImGui::SetNextWindowSize(ImVec2(450.0f, 280.0f), ImGuiCond_FirstUseEver);

                    if (ImGui::Begin("File Explorer", &showFileExplorer))
                    {
                        ImGui::Text("C:\\ROBCO\\SYSTEM32");
                        ImGui::Separator();
                        if (ImGui::TreeNode("Documents")) {
                            ImGui::BulletText("sample.TXT");
                            ImGui::BulletText("group5.DAT");
                            ImGui::TreePop();
                        }
                        if (ImGui::TreeNode("System Files")) {
                            ImGui::BulletText("BOOT.INI");
                            ImGui::BulletText("KERNEL.SYS");
                            ImGui::TreePop();
                        }
                    }
                    ImGui::End();
                }

                if (showTaskManager)
                {
                    ImGui::SetNextWindowSize(ImVec2(600.0f, 350.0f), ImGuiCond_FirstUseEver);
                    if (ImGui::Begin("Task Manager", &showTaskManager))
                    {
                        ImGui::Text("TASK MANAGER");
                        ImGui::Separator();
                        ImGui::Text("Processes");
                        ImGui::Spacing();

                        ImGuiTableFlags tableFlag = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;
                        if (ImGui::BeginTable("ProcessTable", 3, tableFlag))
                        {
                            ImGui::TableSetupColumn("Process");
                            ImGui::TableSetupColumn("CPU");
                            ImGui::TableSetupColumn("Memory");
                            ImGui::TableHeadersRow();

                            const char* processes[][3] =
                            {
                                { "Fallout4.exe",           "3.2%",  "84 MB" },
                                { "Explorer.exe",           "1.4%",  "62 MB" },
                                { "LeagueOfLegends.exe",    "0.8%",  "31 MB" },
                                { "CSOPESY.exe",            "2.1%",  "48 MB" },
                                { "System",                 "0.5%",  "24 MB" }
                            };

                            for (int i = 0; i < 5; i++)
                            {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(processes[i][0]);
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextUnformatted(processes[i][1]);
                                ImGui::TableSetColumnIndex(2);
                                ImGui::TextUnformatted(processes[i][2]);
                            }

                            ImGui::EndTable();
                        }
                    }
                    ImGui::End();
                }
            }
            // End of additions - Luis
        // End of additions - Kyan


        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        //{
        //    static float f = 0.0f;
        //    static int counter = 0;

        //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //    ImGui::Checkbox("Another Window", &show_another_window);

        //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //        counter++;
        //    ImGui::SameLine();
        //    ImGui::Text("counter = %d", counter);

        //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        //    ImGui::End();
        //}

        // 3. Show another simple window.
        //if (show_another_window)
        //{
        //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //    ImGui::Text("Hello from another window!");
        //    if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //    ImGui::End();
        //}

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
