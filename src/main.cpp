
#include "win.h"


#include "interface.h"

#define PY_CODE "import numpy as np\n\
import embeddedmodule"

static char text[1024 * 16] ="pic = np.random.randint(0, high=255, size=(6, 6, 4), dtype='uint8')\n"
                "embeddedmodule.copy3DNumpyArray(pic)";
                

bool imagedata_to_gpu(unsigned char* image_data,  GLuint* out_texture, int image_width, int image_height)
{
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    

    *out_texture = image_texture;
}


int main()
{
    win my_win;
    my_win.init();

    pybind11::scoped_interpreter guard{};

    // auto sys = pybind11::module::import ("sys");
    // sys.attr("path").attr("insert")(1, "/home/wm/Desktop/dev/python_in_cpp/p_in_cpp_5/python");
    // pybind11::print(sys.attr("path"));

    //
    pybind11::exec (PY_CODE);
    pybind11::exec (text);

    GLuint my_image_texture = 0;
    imagedata_to_gpu(myVec3D->flttend3D, &my_image_texture, myVec3D->cols, myVec3D->rows);


    while (my_win.loop())
    {
        pybind11::exec (text);
     
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myVec3D->cols, myVec3D->rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, myVec3D->flttend3D);

        my_win.pre_render();

        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);

        if(ImGui::Begin("text window"))
        {
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
        }
        ImGui::End();

        // render frame buffer output image
        if(ImGui::Begin("image window"))
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddImage((void*)(intptr_t)my_image_texture,
                pos,
                ImVec2(pos.x + size.x, pos.y + size.y),
                ImVec2(0, 1),
                ImVec2(1, 0));
        }
        ImGui::End();

        my_win.imgui_render();
        my_win.opengl_render();
        my_win.swap_buffers();

    }
    matrix3D_free(myVec3D);
    return 0;
}