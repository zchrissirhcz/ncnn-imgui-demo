#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "app_design.hpp"
#include "ncnn_yolox.hpp"

#include "portable-file-dialogs.h"
#include "image_render.hpp"

#define STR_IMPLEMENTATION
#include "Str.h"


using namespace imcmp;

class MyApp : public App<MyApp>
{
public:
    MyApp() = default;
    ~MyApp() = default;

    void myUpdateMouseWheel();

    void StartUp()
    {
        // Title
        glfwSetWindowTitle(window, u8"ncnn imgui demo");
        glfwSetWindowSize(window, 960, 640);
    }

    void Update()
    {
        myUpdateMouseWheel();

        static bool use_work_area = true;
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse;
        
        // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
        // Based on your use case you may want one of the other.
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
        ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);


        ImGui::Begin("Hello, ncnn! Hello, imgui!", NULL, flags);

        // if (ImGui::Button("ncnn-yolox-run"))
        // {
        //     LoadImage(input_image);
        // }

        // if (ImGui::Button("directly run"))
        // {
        //     const char* image_path = "frontground.png";
        //     cv::Mat result = ncnn_yolox_main(image_path);
        //     result_image.load_mat(result);
        // }


        if (ImGui::Button("load image"))
        {
            LoadImage(input_image);
        }
        ImGui::Checkbox("show input", &show_input);
        if (show_input && !input_image.mat.empty())
        {
            ShowImage("input", input_image.get_open(), input_image, 1.0f);
        }


        if (ImGui::Button("run ncnn yolox"))
        {
            if (input_image.mat.empty())
            {
                const char* image_path = "frontground.png";
                cv::Mat result = ncnn_yolox_main(image_path);
                result_image.load_mat(result);
            }
            else
            {
                cv::Mat result = ncnn_yolox_main(input_image.get_name());
                result_image.load_mat(result);
            }
        }
        ImGui::Checkbox("show result", &show_result);
        if (!result_image.mat.empty())
        {
            //ImGui::Text("got result!");
            ShowImage("result", result_image.get_open(), result_image, 1.0f);
        }

        ImGui::End();
    }

public:
    void InitFileFilters();

private:
    int UI_ChooseImageFile();
    void LoadImage(RichImage& image);

    RichImage input_image;
    RichImage result_image;
    Str256 filepath;
    void ShowImage(const char* windowName, bool* open, const RichImage& image, float align_to_right_ratio = 0.f);
    int zoom_percent = 46;
    int zoom_percent_min = 10;
    int zoom_percent_max = 1000;
    std::string filter_msg1 = "Image Files (";
    std::string filter_msg2 = "";

    bool show_input = true;
    bool show_result = true;
};


void MyApp::InitFileFilters()
{
    filter_msg1 = "Image Files (";
    filter_msg2 = "";
    std::vector<std::string> exts = {".jpg", ".jpeg", ".png", ".bmp"};
    for (int i = 0; i < exts.size(); i++)
    {
        if (i > 0)
        {
            filter_msg1 += " ." + exts[i];
            filter_msg2 += " *." + exts[i];
        }
        else
        {
            filter_msg1 += "." + exts[i];
            filter_msg2 += "*." + exts[i];
        }
    }
    filter_msg1 += ")";
}

void MyApp::ShowImage(const char* windowName, bool* open, const RichImage& image, float align_to_right_ratio)
{
    if (*open)
    {
        GLuint texture = image.get_texture();
        //ImGui::SetNextWindowSizeConstraints(ImVec2(500, 500), ImVec2(INFINITY, INFINITY));

        // ImVec2 p_min = ImGui::GetCursorScreenPos(); // actual position
        // ImVec2 p_max = ImVec2(ImGui::GetContentRegionAvail().x + p_min.x, ImGui::GetContentRegionAvail().y  + p_min.y);

        //ImGui::BeginChild("Image1Content", ImVec2(0, 0), true);
        //ImGui::Begin("Image1Content", NULL);
        //ImGui::GetWindowDrawList()->AddImage((void*)(uintptr_t)texture, p_min, p_max);
        //ImGui::EndChild();
        //ImGui::End();
        //
        ImVec2 actual_image_size(image.mat.size().width, image.mat.size().height);
        ImVec2 rendered_texture_size = actual_image_size * (zoom_percent * 1.0 / 100);

        bool clamped_x_by_window = false;
        bool clamped_y_by_window = false;

        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 image_window_size = rendered_texture_size;
        if (rendered_texture_size.x > window_size.x)
        {
            //rendered_texture_size.x = window_size.x;
            clamped_x_by_window = true;
        }
        if (rendered_texture_size.y > window_size.y)
        {
            //rendered_texture_size.y = window_size.y;
            clamped_y_by_window = true;
        }
        bool clamped_by_window = clamped_x_by_window | clamped_y_by_window;
        if (clamped_by_window)
        {
            image_window_size = window_size;
        }

        if (align_to_right_ratio >= 0 && align_to_right_ratio <= 1)
        {
            ImVec2 win_size = ImGui::GetCurrentWindow()->Size;
            ImVec2 offset;
            offset.y = 0;
            if (clamped_x_by_window)
                offset.x = 0;
            else
                offset.x = (win_size.x - rendered_texture_size.x) * align_to_right_ratio;
            ImVec2 p_min = ImGui::GetCursorScreenPos() + offset;
            //ImVec2 p_max = p_min + rendered_texture_size;
            //ImGui::GetWindowDrawList()->AddImage((void*)(uintptr_t)texture, p_min, p_max);
            ImGui::SetNextWindowPos(p_min);
        }
        else
        {
            //ImGui::Image((void*)(uintptr_t)texture, rendered_texture_size);
            ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        }

        //std::string label = cv::format("image##%d", texture);
        Str256 label;
        label.setf("image##%d", texture);
        ImGui::BeginChild(label.c_str(), image_window_size, clamped_by_window, ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImGui::Image((void*)(uintptr_t)texture, rendered_texture_size);
        }
        ImGui::EndChild();
    }
}

static const float WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER = 2.00f; // Lock scrolled window (so it doesn't pick child windows that are scrolling through) for a certain time, unless mouse moved.

static void StartLockWheelingWindow(ImGuiWindow* window)
{
    ImGuiContext& g = *GImGui;
    if (g.WheelingWindow == window)
        return;
    g.WheelingWindow = window;
    g.WheelingWindowRefMousePos = g.IO.MousePos;
    g.WheelingWindowTimer = WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER;
}

// When with mouse wheel moving (vertically), and current window name contains 'Image', resize current window's size
void MyApp::myUpdateMouseWheel()
{
    ImGuiContext& g = *GImGui;

    ImGuiWindow* cur_window = g.WheelingWindow;

    // Reset the locked window if we move the mouse or after the timer elapses
    if (cur_window != NULL)
    {
        g.WheelingWindowTimer -= g.IO.DeltaTime;
        if (ImGui::IsMousePosValid() && ImLengthSqr(g.IO.MousePos - g.WheelingWindowRefMousePos) > g.IO.MouseDragThreshold * g.IO.MouseDragThreshold)
            g.WheelingWindowTimer = 0.0f;
        if (g.WheelingWindowTimer <= 0.0f)
        {
            g.WheelingWindow = NULL;
            g.WheelingWindowTimer = 0.0f;
        }
    }

    float wheel_y = g.IO.MouseWheel;

    if ((g.ActiveId != 0 && g.ActiveIdUsingMouseWheel) || (g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrameUsingMouseWheel))
        return;

    ImGuiWindow* window = g.WheelingWindow ? g.WheelingWindow : g.HoveredWindow;
    if (!window || window->Collapsed)
        return;

    if (wheel_y != 0.0f)
    {
        StartLockWheelingWindow(window);

        zoom_percent = zoom_percent + g.IO.MouseWheel * 5;
        if (zoom_percent > zoom_percent_max)
        {
            zoom_percent = zoom_percent_max;
        }
        else if (zoom_percent < zoom_percent_min)
        {
            zoom_percent = zoom_percent_min;
        }

        return;
    }
}

int MyApp::UI_ChooseImageFile()
{
    // Check that a backend is available
    if (!pfd::settings::available())
    {
        std::cout << "Portable File Dialogs are not available on this platform.\n";
        return 1;
    }

    // Set verbosity to true
    pfd::settings::verbose(true);

    // NOTE: file extension filter not working on macOSX
    auto f = pfd::open_file("Choose image file", pfd::path::home(),
                            //{"Image Files (.jpg .png .jpeg .bmp .nv21 .nv12 .rgb24 .bgr24)", "*.jpg *.png *.jpeg *.bmp *.nv21 *.nv12 *.rgb24 *.bgr24",
                            {filter_msg1, filter_msg2, "All Files", "*"}
                            //pfd::opt::multiselect
    );
    if (f.result().size() > 0)
    {
        std::cout << "Selected files:";
        std::cout << f.result()[0] << std::endl;

        filepath.setf("%s", f.result()[0].c_str());
    }
    return 0;
}

void MyApp::LoadImage(RichImage& image)
{
    UI_ChooseImageFile();
    if (filepath.c_str())
    {
        image.load_from_file(filepath);
    }
    filepath = NULL;
}

int main()
{
    MyApp app;
    app.Run();

    return 0;
}
