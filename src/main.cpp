
#include "win.h"

#include "interface.h"

#define PY_CODE "import numpy as np\n\
import embeddedmodule\n\
array2D= np.array([[1, 2, 3], [4, 5, 6]], np.int32)\n\
array3D = np.array([[[10,20], [30,40], [50,60], [70,80]]], np.int32)\n\
pic = np.random.randint(0, high=255, size=(2, 2, 4), dtype='uint8')\n\
embeddedmodule.copy2DNumpyArray(array2D)\n\
embeddedmodule.copy3DNumpyArray(pic)\n\
print(pic)"


int main()
{
    win my_win;
    my_win.init();

    pybind11::scoped_interpreter guard{};

    // auto sys = pybind11::module::import ("sys");
    // sys.attr("path").attr("insert")(1, "/home/wm/Desktop/dev/python_in_cpp/p_in_cpp_5/python");
    // pybind11::print(sys.attr("path"));

    //auto hello_module = pybind11::module::import("test");
    pybind11::exec (PY_CODE);

    matrix2D_print(myVec2D);
    matrix3D_print(myVec3D);
    matrix2D_free(myVec2D);
    matrix3D_free(myVec3D);

    while (my_win.loop())
    {
        my_win.pre_render();

        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);



        my_win.imgui_render();
        my_win.opengl_render();

        // 3. draw in loop ("without index buffer" version)
        glDrawArrays(GL_TRIANGLES, 0, 3); // draw with it, if we dont have an index buffer (); draws buffer bind with glBindBuffer
        my_win.swap_buffers();

    }

    return 0;
}