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
static ImVec4 s_clear_color{ ImVec4(0.45f, 0.55f, 0.60f, 1.00f) };

const float CLEAR_COLOR_ALPHA[4] = { s_clear_color.x * s_clear_color.w,
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

bool create_d3d_device(HWND, D3DContext &);

HWND win32_windows_handle(SDL_Window *);

SDL_Window *create_sdl_window(const std::string &, const std::size_t &, const std::size_t &);

void cleanup(SDL_Window *tp_sdl_window, D3DContext &tr_d3d_context);
void create_render_target(D3DContext &tr_context);
void destroy_d3d_device(D3DContext &tr_context);
void destroy_render_target(D3DContext &tr_context);
void handle_sdl_event(SDL_Window *tp_sdl_window, AppContext &tr_context);
void initialize_imgui(SDL_Window *tp_sdl_window, D3DContext &tr_context);
void initialize_sdl();
void render(D3DContext &tr_context);

}// namespace TimeBox

#endif// GUI_BACKEND_HPP