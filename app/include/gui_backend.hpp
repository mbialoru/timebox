#ifndef GUI_BACKEND_HPP
#define GUI_BACKEND_HPP

#pragma once

#include <cstdint>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>

namespace TimeBox {

// Variables for ImGui
static const uint32_t max_fps{ 20 };
static uint32_t last_frametime, this_frametime;
static ImVec4 clear_color{ ImVec4(0.45f, 0.55f, 0.60f, 1.00f) };
const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w,
  clear_color.y *clear_color.w,
  clear_color.z *clear_color.w,
  clear_color.w };

struct D3DContext
{
  ID3D11Device *p_d3d_device{ NULL };
  IDXGISwapChain *p_swap_chain{ NULL };
  ID3D11DeviceContext *p_d3d_device_context{ NULL };
  ID3D11RenderTargetView *p_render_target_view{ NULL };
};

bool CreateDeviceD3D(HWND, D3DContext &);
void DestroyDeviceD3D(D3DContext &);
void CreateRenderTarget(D3DContext &);
void DestroyRenderTarget(D3DContext &);

}// namespace TimeBox

#endif// GUI_BACKEND_HPP