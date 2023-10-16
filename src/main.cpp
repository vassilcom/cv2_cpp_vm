
#include <pybind11/embed.h>
#include <iostream>

struct MyData
{
    float x;
    float y;
};

void say_something()
{
    printf("hallo from say_something() \n");
}

static int COUNTER = 0;

void set_counter(int count)
{
    COUNTER = count;
}

// write embedded py module. sort of bridg between py and c++
PYBIND11_EMBEDDED_MODULE (embeddedmodule, module) //second arg: we recieve a module with wich we can expose c++ to python
{
    // give name to module
    module.doc () = "Embedded Module";

    // expose c++ fun to python. give name and fun pointer
    module.def ("say_something", &say_something);
    module.def ("set_counter", &set_counter);

    // map c++ class to py
    pybind11::class_<MyData>(module,"MyData")
    .def_readwrite ("x", &MyData::x)
    .def_readwrite ("y", &MyData::y);
}

int main()
{
    
    pybind11::scoped_interpreter guard{};

    pybind11::exec ("print('hello world with python line')");

    pybind11::exec ("import embeddedmodule");
    pybind11::exec ("embeddedmodule.say_something()");// import embedded module 
    pybind11::exec ("embeddedmodule.set_counter(10)");// import embedded module 

    printf("c++ print counter %d \n",COUNTER);


    //take a look where our py env is running from:
    auto sys = pybind11::module::import ("sys");
    //insert path to the py file. same as: sys.path.insert(0,'/path/to/mod_directory')
    sys.attr("path").attr("insert")(1, "/home/wm/Desktop/dev/python_in_cpp/p_in_cpp_4/python");
    pybind11::print(sys.attr("path"));

    // import file and execute funcs
    auto hello_module = pybind11::module::import("test");
    auto magic_func = hello_module.attr("magic");
    auto doubling_func = hello_module.attr("doubling");
    magic_func(3.4);
    auto fff = doubling_func(12.1);
    float n = fff.cast<float>();
    std::cout<<"fff " << n << "\n";

    MyData data {};
    data.x = 33.3;

    auto magic_func2 = hello_module.attr("magic2");
    magic_func2(data);

    return 0;
}