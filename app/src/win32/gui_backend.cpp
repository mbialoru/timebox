#include "gui_backend.hpp"

using namespace TimeBox;

bool TimeBox::CreateDeviceD3D(HWND t_hwnd, D3DContext &tr_context)
{
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = t_hwnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_0,
  };
  if (D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        createDeviceFlags,
        featureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &sd,
        &(tr_context.p_swap_chain),
        &(tr_context.p_d3d_device),
        &featureLevel,
        &(tr_context.p_d3d_device_context))
      != S_OK)
    return false;

  CreateRenderTarget(tr_context);
  return true;
}

void TimeBox::DestroyDeviceD3D(D3DContext &tr_context)
{
  DestroyRenderTarget(tr_context);
  if (tr_context.p_swap_chain) {
    tr_context.p_swap_chain->Release();
    tr_context.p_swap_chain = NULL;
  }
  if (tr_context.p_d3d_device_context) {
    tr_context.p_d3d_device_context->Release();
    tr_context.p_d3d_device_context = NULL;
  }
  if (tr_context.p_d3d_device) {
    tr_context.p_d3d_device->Release();
    tr_context.p_d3d_device = NULL;
  }
}

void TimeBox::CreateRenderTarget(D3DContext &tr_context)
{
  ID3D11Texture2D *pBackBuffer;
  tr_context.p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  tr_context.p_d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &(tr_context.p_render_target_view));
  pBackBuffer->Release();
}

void TimeBox::DestroyRenderTarget(D3DContext &tr_context)
{
  if (tr_context.p_render_target_view) {
    tr_context.p_render_target_view->Release();
    tr_context.p_render_target_view = NULL;
  }
}

void TimeBox::InitializeSDL()
{
  // SDL Init
  if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER)) {
    BOOST_LOG_TRIVIAL(error) << SDL_GetError();
    throw std::runtime_error("Cannot initialize SDL backend");
  }

  // From SDL 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
}

SDL_Window *
  TimeBox::CreateSDLWindow(const std::string &tr_name, const std::size_t &tr_height, const std::size_t &tr_width)
{
  SDL_WindowFlags window_flags{ static_cast<SDL_WindowFlags>(SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI) };
  SDL_Window *window{ SDL_CreateWindow(
    tr_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, tr_height, tr_width, window_flags) };
  return window;
}

HWND TimeBox::Win32WindowHandle(SDL_Window *tp_sdl_window) {}