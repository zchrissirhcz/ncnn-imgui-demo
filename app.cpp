#define GL_SILENCE_DEPRECATION

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

#include "image_render.hpp"

#define STR_IMPLEMENTATION
#include "Str.h"


using namespace imcmp;

class MyApp : public App<MyApp>
{
public:
    MyApp() = default;
    ~MyApp() = default;

    void StartUp()
    {
        // Title
        glfwSetWindowTitle(window, u8"ncnn imgui demo");
        glfwSetWindowSize(window, 960, 640);
    }

    void Update()
    {
        static bool use_work_area = true;
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse;
        
        // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
        // Based on your use case you may want one of the other.
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
        ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);


        ImGui::Begin("Hello, ncnn! Hello, imgui!", NULL, flags);
        
        if (ImGui::Button("ncnn-yolox-run"))
        {
            const char* image_path = "/Users/zz/data/frontground.png";
            cv::Mat result = ncnn_yolox_main(image_path);
            cv::Mat src = cv::imread(image_path);
            rich_image.load_mat(result);
        }

        if (!rich_image.mat.empty())
        {
            ImGui::Text("got result!");
            ShowImage("result", rich_image.get_open(), rich_image, 1.0f);
        }

        ImGui::End();
    }
private:
    RichImage rich_image;
    void ShowImage(const char* windowName, bool* open, const RichImage& image, float align_to_right_ratio = 0.f);
    int zoom_percent = 46;
};

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


int main()
{
    MyApp app;
    app.Run();

    return 0;
}
