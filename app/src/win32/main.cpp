#include <SDL.h>
#include <SDL_syswm.h>
#include <boost/log/trivial.hpp>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl.h>

#include "application.hpp"
#include "config.hpp"
#include "utilities.hpp"

using namespace TimeBox;

static ID3D11Device *p_d3d_device{ NULL };
static IDXGISwapChain *p_swap_chain{ NULL };
static ID3D11DeviceContext *p_d3d_device_context{ NULL };
static ID3D11RenderTargetView *p_render_target_view{ NULL };

bool CreateDeviceD3D(HWND);
void DestroyDeviceD3D();
void CreateRenderTarget();
void DestroyRenderTarget();

// main()
int wWinMain(HINSTANCE, HINSTANCE, PWSTR, INT)
{
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    BOOST_LOG_TRIVIAL(error) << SDL_GetError();
    return EXIT_FAILURE;
  }

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  // Create window
  SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window *window = SDL_CreateWindow(std::string(BuildInformation::PROJECT_NAME).c_str(),
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_HEIGHT,
    WINDOW_WIDTH,
    window_flags);
  SDL_SysWMinfo wm_info;
  SDL_VERSION(&wm_info.version);
  SDL_GetWindowWMInfo(window, &wm_info);
  HWND window_handle{ wm_info.info.win.window };

  // Initialize Direct3D
  if (not CreateDeviceD3D(window_handle)) {
    DestroyDeviceD3D();
    BOOST_LOG_TRIVIAL(error) << "Failed to create D3D device !";
    return EXIT_FAILURE;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForD3D(window);
  ImGui_ImplDX11_Init(p_d3d_device, p_d3d_device_context);

  // Variables for ImGui
  static const Uint32 max_fps{ 20 };
  static Uint32 last_frametime, this_frametime;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Application context
  AppContext context = InitializeContext();

  // Main loop
  while (context.application_run) {
    // FPS limiter
    last_frametime = this_frametime;
    this_frametime = SDL_GetTicks();

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
      if (event.type == SDL_QUIT) context.application_run = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
          && event.window.windowID == SDL_GetWindowID(window))
        context.application_run = false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // GUI parts
    MainDialog(context);
    ConnectDialog(context);
    WarningPopup(context);
    AboutDialog(context);

    // Rendering - This is our Viewport
    ImGui::Render();
    const float clear_color_with_alpha[4] = {
      clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w
    };
    p_d3d_device_context->OMSetRenderTargets(1, &p_render_target_view, NULL);
    p_d3d_device_context->ClearRenderTargetView(p_render_target_view, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    p_swap_chain->Present(1, 0);// VSync

    // FPS limiter
    if (this_frametime - last_frametime < 1000 / max_fps) SDL_Delay(1000 / max_fps - this_frametime + last_frametime);
  }

  // Cleanup
  DestroyContext(context);

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  DestroyDeviceD3D();
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}

bool CreateDeviceD3D(HWND t_hwnd)
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
        &p_swap_chain,
        &p_d3d_device,
        &featureLevel,
        &p_d3d_device_context)
      != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

void DestroyDeviceD3D()
{
  DestroyRenderTarget();
  if (p_swap_chain) {
    p_swap_chain->Release();
    p_swap_chain = NULL;
  }
  if (p_d3d_device_context) {
    p_d3d_device_context->Release();
    p_d3d_device_context = NULL;
  }
  if (p_d3d_device) {
    p_d3d_device->Release();
    p_d3d_device = NULL;
  }
}

void CreateRenderTarget()
{
  ID3D11Texture2D *pBackBuffer;
  p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  p_d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &p_render_target_view);
  pBackBuffer->Release();
}

void DestroyRenderTarget()
{
  if (p_render_target_view) {
    p_render_target_view->Release();
    p_render_target_view = NULL;
  }
}