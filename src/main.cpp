
#include "win.h"


#include "interface.h"

namespace py = pybind11;

static char src_text[1024 * 20] =R"(
import numpy as np
import cv2
import embeddedmodule

# on re-exec aftr code edit previous capture neds to be released.
# vm_todo: how to get along without this ugly checkout?
if 'capture' in globals():
	capture.release()

capture = cv2.VideoCapture(0)

pic = np.random.randint(0, high=255, size=(6, 6, 4), dtype='uint8')

def onFrame():
	if(capture.isOpened()):
		ret, frame = capture.read()
		dim = (300, 300)
		frame = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)

		# uncomment it for canny edges:
		#frame = cv2.Canny(frame, 100, 200)

		if(ret == True):
			frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
			frame = cv2.cvtColor(frame, cv2.COLOR_BGR2BGRA)
			embeddedmodule.copy3DNumpyArray(frame)
		else:
			print('no frame available')

)";

                
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
	return true;

}

    

py::object global;


py::function get_py_function_and_set_instance_id()
{
	py::function funcc;
	try 
	{
		funcc = py::reinterpret_borrow<py::function>( global["onFrame"]);
	} 
	catch (const py::error_already_set& e) 
	{
		PyErr_Print();
	}
	return funcc;
}


void py_run()
{
	try 
	{
		get_py_function_and_set_instance_id()();
        
	} 
	catch (const py::error_already_set& e) 
	{
		PyErr_Print();
	}
}

void py_rebuild()
{
	try 
	{
		frame_bg = ImVec4(0, 0.2, 0.2, 1.0);
    	py::exec(src_text, global);
	} 
	catch (const py::error_already_set& e) 
	{
		frame_bg = ImVec4(0.2, 0.0, 0.0, 1.0);
		PyErr_Print();
	}
}

int main()
{
	
    win my_win;
    my_win.init();

    pybind11::scoped_interpreter guard{};

	// Create a Python module and execute code from the string
    py::module main = py::module::import("__main__");
    global = main.attr("__dict__");
    py_rebuild();
	py_run();


	if(myVec3D == nullptr)
        myVec3D = matrix3D_create(4, 4,4); // just some black pixels
		
    GLuint my_image_texture = 0;
    imagedata_to_gpu(myVec3D->flttend3D, &my_image_texture, myVec3D->cols, myVec3D->rows);



    while (my_win.loop())
    {
		py_run();


		if(myVec3D->flttend3D)
        	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myVec3D->cols, myVec3D->rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, myVec3D->flttend3D);

        my_win.pre_render();

        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);

	    ImGui::SetNextWindowPos({ 20, 350}, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 700,350 }, ImGuiCond_FirstUseEver);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg);
        if(ImGui::Begin("python loop code"))
        {
			ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            if(ImGui::InputTextMultiline("##source", src_text, IM_ARRAYSIZE(src_text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25),ImGuiInputTextFlags_AllowTabInput))
            {
				py_rebuild();
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
	
    matrix3D_free(myVec3D);
    return 0;
}