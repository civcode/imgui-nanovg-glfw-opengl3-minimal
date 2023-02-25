// dear imgui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"
#include "perf.h"


#include "DrawTest.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

int loadFonts(NVGcontext *vg) {
    int font;

    font = nvgCreateFont(vg, "roboto-regular", "../dep/nanovg/example/Roboto-Regular.ttf");
    if (font == -1) {
        printf("Could not add font regular.\n");
        return -1;
    }
    font = nvgCreateFont(vg, "roboto-medium", "../dep/imgui/misc/fonts/Roboto-Medium.ttf");
    if (font == -1) {
        printf("Could not add font regular.\n");
        return -1;
    }
    // font = nvgCreateFont(vg, "sans-bold", "../dep/nanovg/example/Roboto-Bold.ttf");
    // if (font == -1) {
    //     printf("Could not add font bold.\n");
    //     return -1;
    // }
    return 0;
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    GLFWwindow* window;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	};

    // Set up NanoVG
    NVGcontext* vg = NULL;
	GPUtimer gpuTimer;
	PerfGraph fps, cpuGraph, gpuGraph;
	double prevt = 0, cpuTime = 0;
	NVGLUframebuffer* fb = NULL;
	int winWidth, winHeight;
	int fbWidth, fbHeight;
	float pxRatio;

    initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");
	initGraph(&cpuGraph, GRAPH_RENDER_MS, "CPU Time");
	initGraph(&gpuGraph, GRAPH_RENDER_MS, "GPU Time");

    glfwSetErrorCallback(errorcb);
#ifndef _WIN32 // don't require this on win32, and works with more cards
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

    window = glfwCreateWindow(1280, 720, "Dear ImGui NanoVG+GLFW+OpenGL3", NULL, NULL);
    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

    // Create hi-dpi FBO for hi-dpi screens.
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	// Calculate pixel ration for hi-dpi devices.
	pxRatio = (float)fbWidth / (float)winWidth;

	// The image pattern is tiled, set repeat on x and y.
	fb = nvgluCreateFramebufferGL3(vg, (int)(100*pxRatio), (int)(100*pxRatio), NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
	if (fb == NULL) {
		printf("Could not create FBO.\n");
		return -1;
	}

	if (loadFonts(vg) == -1) {
		printf("Could not load fonts\n");
		return -1;
	}
    

    //glfwSwapInterval(1); // Enable vsync
    glfwSwapInterval(0); // max. FPS

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL2_Init();
    ImGui_ImplOpenGL3_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../dep/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../dep/imgui/misc/fonts/Roboto-Medium.ttf", 17.0f);
    io.Fonts->AddFontFromFileTTF("../dep/nanovg/example/Roboto-Regular.ttf", 17.0f);
    //io.Fonts->AddFontFromFileTTF("../dep/imgui/misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../dep/imgui/misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../dep/imgui/misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    DrawTest draw(window, vg);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        //ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

    
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //show_demo_window = false;
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        
        /*
        //ImGui::Button("myButton");
        //ImGui::Begin("main",nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImGui::Begin("main");
        //ImGui::SetWindowPos("main", ImVec2(10, 0));
        if (ImGui::Button("my button")) {
            printf("test\n");
        }  
        ImGui::Button("my button 1");
        ImGui::Button("my button 2");
        ImGui::End();
        */

        ImGui::Begin("tabs");
        ImGui::BeginTabBar("Settings#left_tabs_bar");
        if (ImGui::BeginTabItem("General")){
            static bool fullscreen = false;
            if(ImGui::Checkbox("Fullscreen Mode", &fullscreen)){
                printf("tab general\n");
            }
            if(ImGui::Checkbox("Enable Multisampling",&fullscreen)){
                printf("tab general\n");
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("GUI")){
            ImGui::Text("Tab 2");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Tab Name")){
            ImGui::Text("Tab 3");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        ImGui::End();
        //ImGui::Dummy(ImVec2(20, 20));


        //ImGui::SetWindowFontScale(1.0);
         
        //NanoVG
        {
            //glfwGetWindowSize(window, &winWidth, &winHeight);
            //glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
            //nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

            draw.draw();

            //nvgEndFrame(vg);
        }
        
        // Rendering
        ImGui::Render();

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        //ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //glUseProgram(last_program);
        

        
        //NanoVG
        if (false) {
            glfwGetWindowSize(window, &winWidth, &winHeight);
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
            //nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
            //nvgSave(vg);
            //draw.draw();

            if (false) {
                double mx, my;
                float zoom = 1;

                glfwGetCursorPos(window, &mx, &my);
                // cursor
                nvgSave(vg);
                nvgTranslate(vg, mx, my); //move offset of CS to [mx. my]
                nvgScale(vg, zoom, zoom);
                nvgBeginPath(vg);
                nvgMoveTo(vg, 0, 0);
                nvgLineTo(vg, 20, 0);
                nvgMoveTo(vg, 0, 0);
                nvgLineTo(vg, 0, 20);
                nvgCircle(vg, 20, 20, 2);
                nvgStrokeColor(vg, nvgRGBAf(1,1,1,1));
                nvgStrokeWidth(vg, 2);
                nvgStroke(vg);
                nvgClosePath(vg);
            }

            // nvgRestore(vg);

            // nvgBeginPath(vg);
            // nvgFontSize(vg, 14.5);
            // //nvgFontSize(vg, 17.0);
            // nvgFontFace(vg, "roboto-regular");
            // nvgTextAlign(vg, NVG_ALIGN_TOP);
            // //nvgFontBlur(vg, 0.1);
            // nvgFillColor(vg, nvgRGBf(0,0,0));
            // nvgText(vg, 50, 100, "Hello, world!", nullptr);
            // nvgFontFace(vg, "roboto-medium");
            // nvgText(vg, 50, 120, "Hello, world!", nullptr);
            // nvgText(vg, 50, 160, "Demo Window", nullptr);
            // nvgRect(vg, 30, 120, 17, 17);
            // nvgRect(vg, 50, 140, 17, 17);
            // nvgFill(vg); 
            // nvgClosePath(vg);


            //nvgRestore(vg);

            //nvgRestore(vg);


            //nvgEndFrame(vg);
        }

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
