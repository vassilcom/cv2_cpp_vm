
#include "win.h"


#include "interface.h"

#define PY_CODE_BEGIN "import numpy as np\n\
import cv2\n\
import embeddedmodule\n\
capture = cv2.VideoCapture(0)\n\
"
#define PY_CODE_END "capture.release()\n\
print('end of cv2')\n\
"

static char text[1024 * 16] ="\
pic = np.random.randint(0, high=255, size=(6, 6, 4), dtype='uint8')\n\
if(capture.isOpened()):\n\
\tret, frame = capture.read()\n\
\tdim = (300, 300)\n\
\tframe = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)\n\
\n\
\t# uncomment it for canny edges:\n\
\t#frame = cv2.Canny(frame, 100, 200)\n\
\n\
\tif(ret == True):\n\
\t\tframe = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)\n\
\t\tframe = cv2.cvtColor(frame, cv2.COLOR_BGR2BGRA)\n\
\t\tprint(frame)\n\
\t\tembeddedmodule.copy3DNumpyArray(frame)\n\
\telse:\n\
\t\tprint('no frame available')\n\
else:\n\
\tprint('no capture available. make sure that device ID is correct')\n\
";
                
ImVec4 frame_bg = ImVec4(0, 0.2, 0.2,1);


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

bool exec_py()
{
    try
    {
        frame_bg =  ImVec4(0, 0.2, 0.2,1);
        pybind11::exec (text);
        return true;
    }
    catch (...)
    {
        frame_bg = ImVec4(0.2, 0, 0,1);
        return false;
    }
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
    pybind11::exec (PY_CODE_BEGIN);
    exec_py();

    GLuint my_image_texture = 0;
    imagedata_to_gpu(myVec3D->flttend3D, &my_image_texture, myVec3D->cols, myVec3D->rows);


    while (my_win.loop())
    {
        exec_py();

     
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myVec3D->cols, myVec3D->rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, myVec3D->flttend3D);

        my_win.pre_render();

        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);

	    ImGui::SetNextWindowPos({ 20, 350}, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 700,350 }, ImGuiCond_FirstUseEver);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg);
        if(ImGui::Begin("python loop code"))
        {
            if(ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 20),ImGuiInputTextFlags_AllowTabInput))
            {
            }

        }
        ImGui::End();
        ImGui::PopStyleColor();

        // render frame buffer output image
	    ImGui::SetNextWindowPos({ 10,10 }, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 320,320 }, ImGuiCond_FirstUseEver);
        if(ImGui::Begin("image output"))
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddImage((void*)(intptr_t)my_image_texture,
                pos,
                ImVec2(pos.x + size.x, pos.y + size.y),
                ImVec2(0, 0),
                ImVec2(1, 1));
        }
        ImGui::End();

        my_win.imgui_render();
        my_win.opengl_render();
        my_win.swap_buffers();

    }
    pybind11::exec (PY_CODE_END);
    matrix3D_free(myVec3D);
    return 0;
}