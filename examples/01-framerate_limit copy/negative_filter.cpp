//#define strdup _strdup
#include <pybind11/pybind11.h>
#include <reshade.hpp>
#include <pybind11/embed.h> // everything needed for embedding
#include <iostream>
#include <vector>
#include <pybind11/stl.h>

using namespace reshade::api;
namespace py = pybind11;

/// <summary>
/// ReShade addon callback.
/// See reshade::addon_event::present
/// </summary>
static void on_present(command_queue *queue, swapchain *swapchain, const rect *, const rect *, uint32_t, const rect *)
{
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
	//py::module calc = py::module::import("calc");

	// Invert colors.
	auto mapped_data = static_cast<uint8_t *>(mapped.data);
	for (uint32_t y = 0; y < desc.texture.height; ++y)
	{
		for (uint32_t x = 0; x < desc.texture.width; ++x)
		{
			const uint32_t pixel_index = y * mapped.row_pitch + x * 4; // Assuming RGBA format.
			mapped_data[pixel_index + 0] = 255 - mapped_data[pixel_index + 0]; // R
			mapped_data[pixel_index + 1] = 255 - mapped_data[pixel_index + 1]; // G
			mapped_data[pixel_index + 2] = 255 - mapped_data[pixel_index + 2]; // B
			// Alpha channel (mapped_data[pixel_index + 3]) is left unchanged.
		}
	}

	// Unmap staging texture.
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

/// <summary>
/// Registers ReShade addon callbacks.
/// </summary>
void register_events()
{
	reshade::register_event<reshade::addon_event::present>(on_present);
}

/// <summary>
/// Unregisters ReShade addon callbacks.
/// </summary>
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
// with inversion on max settings 3050ti - 50fps
// off inversion on max settings 3050ti - 72fps
// addon off on max settings 3050ti - 90fps 
