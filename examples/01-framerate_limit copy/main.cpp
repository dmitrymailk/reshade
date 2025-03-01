#include <windows.h>
#include <pybind11/embed.h> // everything needed for embedding
#include <iostream>
#include <vector>
#include <pybind11/stl.h>
#include <reshade.hpp>
// /home/user-name-goes-here/.local/lib/python3.11/site-packages/pybind11
namespace py = pybind11;
using namespace reshade::api;
void import_modules()
{
	py::module calc = py::module::import("calc");
	// create dump matrix
	std::vector<std::vector<int>> matrix = {
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9} };
	// change matrix in cpp
	matrix[0][0] = 1000;
	py::object result = calc.attr("add")(1, 20, matrix);
	auto n = result.cast<std::vector<std::vector<int>>>();
	std::cout << n[0][0] << std::endl;
	// we changed array in python
	assert(n[0][0] != 1000);
}

int main()
{
	py::scoped_interpreter guard {}; // start the interpreter and keep it alive
	import_modules();
}
