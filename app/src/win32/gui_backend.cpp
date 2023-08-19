#include "gui_backend.hpp"

using namespace TimeBox;

void TimeBox::cleanup(SDL_Window *tp_sdl_window, D3DContext &tr_d3d_context)
{
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  destroy_d3d_device(tr_d3d_context);
  SDL_DestroyWindow(tp_sdl_window);
  SDL_Quit();
}

void TimeBox::render(D3DContext &tr_d3d_context)
{
  ImGui::Render();
  tr_d3d_context.p_d3d_device_context->OMSetRenderTargets(1, &(tr_d3d_context.p_render_target_view), NULL);
  tr_d3d_context.p_d3d_device_context->ClearRenderTargetView(
    tr_d3d_context.p_render_target_view, CLEAR_COLOR_ALPHA);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  tr_d3d_context.p_swap_chain->Present(1, 0);// VSync
}

bool TimeBox::create_d3d_device(HWND t_hwnd, D3DContext &tr_context)
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

  create_render_target(tr_context);
  return true;
}

void TimeBox::destroy_d3d_device(D3DContext &tr_context)
{
  destroy_render_target(tr_context);

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

void TimeBox::create_render_target(D3DContext &tr_context)
{
  ID3D11Texture2D *p_back_buffer;

  tr_context.p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&p_back_buffer));
  tr_context.p_d3d_device->CreateRenderTargetView(p_back_buffer, NULL, &(tr_context.p_render_target_view));
  pBackBuffer->Release();
}

void TimeBox::destroy_render_target(D3DContext &tr_context)
{
  if (tr_context.p_render_target_view) {
    tr_context.p_render_target_view->Release();
    tr_context.p_render_target_view = NULL;
  }
}

void TimeBox::initialize_sdl()
{
  // SDL Init
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER)) {
    BOOST_LOG_TRIVIAL(error) << SDL_GetError();
    throw std::runtime_error("Cannot initialize SDL backend");
  }

  // From SDL 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
}

SDL_Window *
  TimeBox::create_sdl_window(const std::string &tr_name, const std::size_t &tr_height, const std::size_t &tr_width)
{
  SDL_WindowFlags window_flags{ static_cast<SDL_WindowFlags>(SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI) };
  SDL_Window *window{ SDL_CreateWindow(
    tr_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, tr_height, tr_width, window_flags) };

  return window;
}

HWND TimeBox::win32_windows_handle(SDL_Window *tp_sdl_window)
{
  SDL_SysWMinfo wm_info;

  SDL_VERSION(&wm_info.version);
  SDL_GetWindowWMInfo(tp_sdl_window, &wm_info);

  HWND win32_window_handle{ wm_info.info.win.window };

  return win32_window_handle;
}

void TimeBox::handle_sdl_event(SDL_Window *tp_sdl_window, AppContext &tr_app_context)
{
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your
  // inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite
  // your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or
  // clear/overwrite your copy of the keyboard data. Generally you may always pass all inputs to dear imgui, and hide
  // them from your application based on those two flags.
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      tr_app_context.application_run = true;
      break;

    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(tp_sdl_window))
        tr_app_context.application_run = false;
      break;

    default:
      break;
    }
  }
}

void TimeBox::initialize_imgui(SDL_Window *tp_sdl_window, D3DContext &tr_d3d_context)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  // ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForD3D(tp_sdl_window);
  ImGui_ImplDX11_Init(tr_d3d_context.p_d3d_device, tr_d3d_context.p_d3d_device_context);
}