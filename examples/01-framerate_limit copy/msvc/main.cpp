#include <windows.h>
#include <Python.h>
#include <pybind11/pybind11.h>
#include <reshade.hpp>
#include <pybind11/embed.h> 
#include <iostream>
#include <vector>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>


using namespace reshade::api;
namespace py = pybind11;

static std::unique_ptr<py::scoped_interpreter> g_python_interpreter;
py::module calc;



static void on_present(command_queue *queue, swapchain *swapchain, const rect *, const rect *, uint32_t, const rect *)
{

	if (!g_python_interpreter)
	{
		// Initialize Python on first use
		g_python_interpreter = std::make_unique<py::scoped_interpreter>();
		py::module sys = py::module::import("sys");
		py::object path = sys.attr("path");
		path.attr("insert")(0, "C:\\Users\\dimweb\\AppData\\Local\\Programs\\Python\\Python311-32\\Lib\\site-packages\\");
	}
	// py::gil_scoped_acquire acquire;
	calc = py::module_::import("calc");

	// Get the device.
	device *const device = swapchain->get_device();
	// Get the back buffer.
	resource back_buffer = swapchain->get_current_back_buffer();
	// Get back buffer description.
	const resource_desc desc = device->get_resource_desc(back_buffer);

	// Create staging texture.
	resource st_texture;
	if (!device->create_resource(
		resource_desc(desc.texture.width, desc.texture.height, 1, 1, desc.texture.format, 1, memory_heap::gpu_to_cpu, resource_usage::copy_dest),
		nullptr,
		resource_usage::copy_dest,
		&st_texture))
	{
		reshade::log::message(reshade::log::level::warning, "Failed to create staging texture.");
		return;
	}

	// Copy back buffer to staging texture.
	command_list *const cmd_list = queue->get_immediate_command_list();
	cmd_list->barrier(back_buffer, resource_usage::present, resource_usage::copy_source);
	cmd_list->copy_resource(back_buffer, st_texture);
	cmd_list->barrier(back_buffer, resource_usage::copy_source, resource_usage::present);
	queue->flush_immediate_command_list();
	queue->wait_idle();

	// Map staging texture.
	subresource_data mapped = {};
	if (!device->map_texture_region(st_texture, 0, nullptr, map_access::read_write, &mapped))
	{
		reshade::log::message(reshade::log::level::warning, "Failed to map staging texture.");
		device->destroy_resource(st_texture);
		return;
	}

	// Invert colors.
	auto mapped_data = static_cast<uint8_t *>(mapped.data);
	// std::vector<int> destination;
	uint32_t data_size = (uint32_t)desc.texture.height * (uint32_t)desc.texture.width * 4;
	// destination.resize(data_size);
	// std::copy(mapped_data, mapped_data + data_size, destination.begin());

	// invertion via c++
	// for (uint32_t y = 0; y < desc.texture.height; ++y)
	// {
	// 	for (uint32_t x = 0; x < desc.texture.width; ++x)
	// 	{
	// 		const uint32_t pixel_index = y * mapped.row_pitch + x * 4; // Assuming RGBA format.
	// 		mapped_data[pixel_index + 0] = 255 - mapped_data[pixel_index + 0]; // R
	// 		mapped_data[pixel_index + 1] = 255 - mapped_data[pixel_index + 1]; // G
	// 		mapped_data[pixel_index + 2] = 255 - mapped_data[pixel_index + 2]; // B
	// 	}
	// }

	// Create a NumPy array that points to the existing data.
	py::array_t<uint8_t> numpy_array(
		// { (int)desc.texture.height, (int)desc.texture.width, 4 }, // Shape: (height, width, channels)
		{ (int)desc.texture.height * (int)desc.texture.width * 4 }, // Shape: (height, width, channels)
		{ 1 },        // Strides: bytes between rows, bytes between columns, bytes between channels
		mapped_data,                            // Pointer to the data
		py::capsule(mapped_data, [](void *data) { /* Do nothing.  Resource is managed externally by ReShade. */ }));

	py::array_t<uint8_t> result = calc.attr("add")(numpy_array);
	std::memcpy(mapped_data, result.data(), data_size);
	device->unmap_texture_region(st_texture, 0);

	// Copy staging texture back to back buffer.
	cmd_list->barrier(back_buffer, resource_usage::present, resource_usage::copy_dest);
	cmd_list->copy_resource(st_texture, back_buffer);
	cmd_list->barrier(back_buffer, resource_usage::copy_dest, resource_usage::present);
	queue->flush_immediate_command_list();
	queue->wait_idle();

	// Destroy staging texture.
	device->destroy_resource(st_texture);
}


void register_events()
{

	reshade::register_event<reshade::addon_event::present>(on_present);
}


void unregister_events()
{
	reshade::unregister_event<reshade::addon_event::present>(on_present);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (!reshade::register_addon(hModule))
		{
			return FALSE;
		}
		register_events();
		break;
	case DLL_PROCESS_DETACH:
		unregister_events();
		reshade::unregister_addon(hModule);
		break;
	}
	return TRUE;
}
