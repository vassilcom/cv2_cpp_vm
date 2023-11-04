
#include "win.h"


#include "interface.h"

#define PY_CODE_BEGIN "import numpy as np\n\
import embeddedmodule\n\
"
#define PY_CODE_END ""
// >>> import cmake_example
// >>> cmake_example.add(4,5)
static char text[1024 * 16] =R"(
pic = np.random.randint(0, high=255, size=(6, 6, 4), dtype='uint8')
#embeddedmodule.copy3DNumpyArray(pic)
)";

static char virt_file[1024 * 16] =R"(
import numpy as np
import cv2
import embeddedmodule
x = 42
y = 5
def fun1():
	pic = np.random.randint(0, high=255, size=(6, 6, 4), dtype='uint8')
	embeddedmodule.copy3DNumpyArray(pic)
	print("fun1 >> dd")
	return 2*2
xxx=3
print("dd")
)";

                
ImVec4 frame_bg = ImVec4(0, 0.2, 0.2,1);

#include <Python.h>
PyObject* myFunction;
bool imagedata_to_gpu(unsigned char* image_data,  GLuint* out_texture, int image_width, int image_height)
{
	if(image_data)
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
	return false;

}

bool exec_py()
{
    try
    {
        frame_bg =  ImVec4(0, 0.2, 0.2,1);
        PyObject* myResult = PyObject_CallObject(myFunction, NULL);// getting result
		float result = PyFloat_AsDouble(myResult);
		printf("fun1: %.3f \n", result);
        return true;
    }
    catch (...)
    {
        frame_bg = ImVec4(0.2, 0, 0,1);
        return false;
    }
}
    
bool rebuild_py()
{
    try
    {
        frame_bg =  ImVec4(0, 0.2, 0.2,1);

		PyObject* compiled_code = Py_CompileString(virt_file, "<string>", Py_file_input);
		
		PyObject* myModule = PyImport_ExecCodeModule("mymodule", compiled_code);
		myFunction = PyObject_GetAttrString(myModule,(char*)"fun1");
		PyObject* myResult = PyObject_CallObject(myFunction, NULL);// getting result
		

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
    //pybind11::exec (PY_CODE_BEGIN);
    //exec_py();
	//PyObject* module_code = "x = 42";
	Py_Initialize();

	rebuild_py();
	exec_py();

    GLuint my_image_texture = 0;
    imagedata_to_gpu(myVec3D->flttend3D, &my_image_texture, myVec3D->cols, myVec3D->rows);




// 	const char *MyModuleName = "blah";
// 	const char *MyModuleCode = R"(
// print( 'Hello world!')
// pint('ddd')
// def fun1():
// 	return 2*2
// )";
// 	PyObject *pyModule = PyModule_New(MyModuleName);
// 	// Set properties on the new module object
// 	PyModule_AddStringConstant(pyModule, "__file__", "");
// 	PyObject *localDict = PyModule_GetDict(pyModule);   // Returns a borrowed reference: no need to Py_DECREF() it once we are done
// 	PyObject *builtins = PyEval_GetBuiltins();  // Returns a borrowed reference: no need to Py_DECREF() it once we are done
// 	PyDict_SetItemString(localDict, "__builtins__", builtins);

// 	// Define code in the newly created module
// 	PyObject *pyValue = PyRun_String(MyModuleCode, Py_file_input, localDict, localDict);
// 	if (pyValue == NULL) {
// 		// Handle error
// 	}
// 	else
// 		Py_DECREF(pyValue);


    while (my_win.loop())
    {


        exec_py();

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

            if(ImGui::InputTextMultiline("##source", virt_file, IM_ARRAYSIZE(virt_file), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 22),ImGuiInputTextFlags_AllowTabInput))
            {
				rebuild_py();
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