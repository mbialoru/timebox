#ifndef GUI_BACKEND_HPP
#define GUI_BACKEND_HPP

#pragma once

#include <SDL.h>
#include <SDL_syswm.h>
#include <boost/log/trivial.hpp>
#include <cstdint>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl.h>
#include <stdexcept>

#include "appcontext.hpp"

namespace TimeBox {

// Variables for ImGui
static const uint32_t s_max_fps{ 20 };
static ImVec4 s_clear_color{ ImVec4(0.45f, 0.55f, 0.60f, 1.00f) };
const float clear_color_with_alpha[4] = { s_clear_color.x * s_clear_color.w,
  s_clear_color.y *s_clear_color.w,
  s_clear_color.z *s_clear_color.w,
  s_clear_color.w };

struct D3DContext
{
  ID3D11Device *p_d3d_device{ NULL };
  IDXGISwapChain *p_swap_chain{ NULL };
  ID3D11DeviceContext *p_d3d_device_context{ NULL };
  ID3D11RenderTargetView *p_render_target_view{ NULL };
};

void Cleanup(SDL_Window *, D3DContext &);
void Render(D3DContext &);

// D3D helper functions
bool CreateDeviceD3D(HWND, D3DContext &);
void DestroyDeviceD3D(D3DContext &);
void CreateRenderTarget(D3DContext &);
void DestroyRenderTarget(D3DContext &);

// SDL helper functions
void InitializeSDL();
SDL_Window *CreateSDLWindow(const std::string &, const std::size_t &, const std::size_t &);
HWND Win32WindowHandle(SDL_Window *);
void HandleSDLEvent(SDL_Window *, AppContext &);

// Dear ImGUI helper function
void InitializeImGUI(SDL_Window *, D3DContext &);

}// namespace TimeBox

#endif// GUI_BACKEND_HPP