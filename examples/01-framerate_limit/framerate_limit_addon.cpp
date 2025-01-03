/*
 * Copyright (C) 2024 Your Name
 * SPDX-License-Identifier: BSD-3-Clause OR MIT
 */

#include <imgui.h>
#include <reshade.hpp>
#include <string>

using namespace reshade::api;

struct __declspec(uuid("7251932A-ADAF-4DFC-B5CB-9A4E8CD5D6EB")) device_data_backbuffer
{
	effect_runtime *main_runtime = nullptr;
};

struct __declspec(uuid("036CD16B-E823-4D6C-A137-5C335D6FD3E6")) command_list_data_backbuffer
{
	resource_view current_back_buffer_rtv = { 0 };
};

static void on_init_device_backbuffer(device *device)
{
	device->create_private_data<device_data_backbuffer>();
}

static void on_destroy_device_backbuffer(device *device)
{
	device->destroy_private_data<device_data_backbuffer>();
}

static void on_init_command_list_backbuffer(command_list *cmd_list)
{
	cmd_list->create_private_data<command_list_data_backbuffer>();
}

static void on_destroy_command_list_backbuffer(command_list *cmd_list)
{
	cmd_list->destroy_private_data<command_list_data_backbuffer>();
}

static void on_init_effect_runtime_backbuffer(effect_runtime *runtime)
{
	auto &dev_data = runtime->get_device()->get_private_data<device_data_backbuffer>();
	if (std::addressof(dev_data) == nullptr)
		return;

	// Assume last created effect runtime is the main one
	dev_data.main_runtime = runtime;
}

static void on_destroy_effect_runtime_backbuffer(effect_runtime *runtime)
{
	auto &dev_data = runtime->get_device()->get_private_data<device_data_backbuffer>();
	if (std::addressof(dev_data) == nullptr)
		return;

	if (runtime == dev_data.main_runtime)
		dev_data.main_runtime = nullptr;
}

static void on_bind_render_targets_and_depth_stencil_backbuffer(command_list *cmd_list, uint32_t count, const resource_view *rtvs, resource_view)
{
	auto &data = cmd_list->get_private_data<command_list_data_backbuffer>();
	data.current_back_buffer_rtv = (count > 0) ? rtvs[0] : resource_view { 0 };
}


static const char* format_to_string(format fmt)
{
	switch (fmt)
	{
	case format::unknown: return "unknown";
	case format::r8_unorm: return "r8_unorm";
	case format::r8_snorm: return "r8_snorm";
	case format::r8_uint: return "r8_uint";
	case format::r8_sint: return "r8_sint";
	case format::a8_unorm: return "a8_unorm";
	case format::l8_unorm: return "l8_unorm";
	case format::r16_unorm: return "r16_unorm";
	case format::r16_snorm: return "r16_snorm";
	case format::r16_uint: return "r16_uint";
	case format::r16_sint: return "r16_sint";
	case format::l16_unorm: return "l16_unorm";
	case format::s8_uint: return "s8_uint";
	case format::r8g8_unorm: return "r8g8_unorm";
	case format::r8g8_snorm: return "r8g8_snorm";
	case format::r8g8_uint: return "r8g8_uint";
	case format::r8g8_sint: return "r8g8_sint";
	case format::r10g10b10a2_unorm: return "r10g10b10a2_unorm";
	case format::r10g10b10a2_uint: return "r10g10b10a2_uint";
	case format::r32_uint: return "r32_uint";
	case format::r32_sint: return "r32_sint";
	case format::r16g16_unorm: return "r16g16_unorm";
	case format::r16g16_snorm: return "r16g16_snorm";
	case format::r16g16_uint: return "r16g16_uint";
	case format::r16g16_sint: return "r16g16_sint";
	case format::r8g8b8a8_unorm: return "r8g8b8a8_unorm";
	case format::r8g8b8a8_snorm: return "r8g8b8a8_snorm";
	case format::r8g8b8a8_uint: return "r8g8b8a8_uint";
	case format::r8g8b8a8_sint: return "r8g8b8a8_sint";
	case format::b8g8r8a8_unorm: return "b8g8r8a8_unorm";
	case format::b8g8r8x8_unorm: return "b8g8r8x8_unorm";
	case format::b10g10r10a2_unorm: return "b10g10r10a2_unorm";
	case format::r16g16b16a16_unorm: return "r16g16b16a16_unorm";
	case format::r16g16b16a16_snorm: return "r16g16b16a16_snorm";
	case format::r16g16b16a16_uint: return "r16g16b16a16_uint";
	case format::r16g16b16a16_sint: return "r16g16b16a16_sint";
	case format::r32g32_uint: return "r32g32_uint";
	case format::r32g32_sint: return "r32g32_sint";
	case format::r32g32b32_uint: return "r32g32b32_uint";
	case format::r32g32b32_sint: return "r32g32b32_sint";
	case format::r32g32b32a32_uint: return "r32g32b32a32_uint";
	case format::r32g32b32a32_sint: return "r32g32b32a32_sint";
	case format::d16_unorm: return "d16_unorm";
	case format::d24_unorm_s8_uint: return "d24_unorm_s8_uint";
	case format::bc1_unorm: return "bc1_unorm";
	case format::bc1_unorm_srgb: return "bc1_unorm_srgb";
	case format::bc2_unorm: return "bc2_unorm";
	case format::bc2_unorm_srgb: return "bc2_unorm_srgb";
	case format::bc3_unorm: return "bc3_unorm";
	case format::bc3_unorm_srgb: return "bc3_unorm_srgb";
	case format::bc4_unorm: return "bc4_unorm";
	case format::bc4_snorm: return "bc4_snorm";
	case format::bc5_unorm: return "bc5_unorm";
	case format::bc5_snorm: return "bc5_snorm";
	case format::bc7_unorm: return "bc7_unorm";
	case format::bc7_unorm_srgb: return "bc7_unorm_srgb";
	case format::bc6h_ufloat: return "bc6h_ufloat";
	case format::bc6h_sfloat: return "bc6h_sfloat";
	default: return "unknown";
	}
}

static void on_reshade_present_backbuffer(effect_runtime *runtime)
{
	device *const device = runtime->get_device();
	auto &dev_data = device->get_private_data<device_data_backbuffer>();
	if (std::addressof(dev_data) == nullptr || runtime != dev_data.main_runtime)
		return;

	command_list *const cmd_list = runtime->get_command_queue()->get_immediate_command_list();
	if (cmd_list == nullptr)
		return;

	const auto &cmd_data = cmd_list->get_private_data<command_list_data_backbuffer>();
	if (cmd_data.current_back_buffer_rtv == 0)
		return;

	resource back_buffer_resource = device->get_resource_from_view(cmd_data.current_back_buffer_rtv);
	resource_desc back_buffer_desc = device->get_resource_desc(back_buffer_resource);


	//reshade::log::message(reshade::log::level::info, format_to_string(back_buffer_desc.texture.format));

	subresource_data mapped_data;



	if (back_buffer_desc.texture.format == format::b8g8r8a8_unorm)
	{
		reshade::log::message(reshade::log::level::warning, "Your texture format is b8g8r8a8_unorm");
		subresource_data mapped_data;
		if (device->map_texture_region(back_buffer_resource, 0, nullptr, map_access::write_only, &mapped_data))
		{
			uint8_t *pixels = static_cast<uint8_t *>(mapped_data.data);
			const uint32_t width = back_buffer_desc.texture.width;
			const uint32_t height = back_buffer_desc.texture.height;

			// Example: Invert the colors for b8g8r8a8_unorm
			for (uint32_t y = 0; y < height; ++y)
			{
				uint8_t *row = pixels + y * mapped_data.row_pitch;
				for (uint32_t x = 0; x < width; ++x)
				{
					uint8_t *pixel = row + x * 4; // Still 4 bytes per pixel
					pixel[0] = 255 - pixel[0]; // B
					pixel[1] = 255 - pixel[1]; // G
					pixel[2] = 255 - pixel[2]; // R
					// pixel[3] remains the same (A)
				}
			}

			device->unmap_texture_region(back_buffer_resource, 0);
		}
	}
	else
	{
		// Log a warning for other formats
		reshade::log::message(reshade::log::level::warning, "Back buffer format is not b8g8r8a8_unorm. Simple modification may not work as expected.");
	}
}

static void on_draw_settings_backbuffer(effect_runtime *runtime)
{
	device *const device = runtime->get_device();
	auto &dev_data = device->get_private_data<device_data_backbuffer>();

	if (std::addressof(dev_data) == nullptr || runtime != dev_data.main_runtime)
	{
		ImGui::TextUnformatted("This is not the main effect runtime.");
		return;
	}

	ImGui::TextUnformatted("This addon modifies the back buffer directly.");
	ImGui::TextUnformatted("Currently, it inverts the colors of the back buffer.");
	ImGui::TextUnformatted("Note: This is a basic example and might not work correctly with all back buffer formats.");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (!reshade::register_addon(hModule))
			return FALSE;

		reshade::register_event<reshade::addon_event::init_device>(on_init_device_backbuffer);
		reshade::register_event<reshade::addon_event::destroy_device>(on_destroy_device_backbuffer);
		reshade::register_event<reshade::addon_event::init_command_list>(on_init_command_list_backbuffer);
		reshade::register_event<reshade::addon_event::destroy_command_list>(on_destroy_command_list_backbuffer);
		reshade::register_event<reshade::addon_event::init_effect_runtime>(on_init_effect_runtime_backbuffer);
		reshade::register_event<reshade::addon_event::destroy_effect_runtime>(on_destroy_effect_runtime_backbuffer);

		reshade::register_event<reshade::addon_event::bind_render_targets_and_depth_stencil>(on_bind_render_targets_and_depth_stencil_backbuffer);
		reshade::register_event<reshade::addon_event::reshade_present>(on_reshade_present_backbuffer);

		reshade::register_overlay(nullptr, on_draw_settings_backbuffer);
		break;
	case DLL_PROCESS_DETACH:
		reshade::unregister_addon(hModule);
		break;
	}

	return TRUE;
}
