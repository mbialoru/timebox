#include "gui_backend.hpp"

using namespace TimeBox;

bool TimeBox::CreateDeviceD3D(HWND t_hwnd, D3DContext &t_context)
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
        &(t_context.p_swap_chain),
        &(t_context.p_d3d_device),
        &featureLevel,
        &(t_context.p_d3d_device_context))
      != S_OK)
    return false;

  CreateRenderTarget(t_context);
  return true;
}

void TimeBox::DestroyDeviceD3D(D3DContext &t_context)
{
  DestroyRenderTarget(t_context);
  if (t_context.p_swap_chain) {
    t_context.p_swap_chain->Release();
    t_context.p_swap_chain = NULL;
  }
  if (t_context.p_d3d_device_context) {
    t_context.p_d3d_device_context->Release();
    t_context.p_d3d_device_context = NULL;
  }
  if (t_context.p_d3d_device) {
    t_context.p_d3d_device->Release();
    t_context.p_d3d_device = NULL;
  }
}

void TimeBox::CreateRenderTarget(D3DContext &t_context)
{
  ID3D11Texture2D *pBackBuffer;
  t_context.p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  t_context.p_d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &(t_context.p_render_target_view));
  pBackBuffer->Release();
}

void TimeBox::DestroyRenderTarget(D3DContext &t_context)
{
  if (t_context.p_render_target_view) {
    t_context.p_render_target_view->Release();
    t_context.p_render_target_view = NULL;
  }
}