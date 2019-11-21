#include "Graphics.h"
#include "ExceptionMacros.h"
#include "DxgiInfoManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "Surface.h"
#include "ThreadSettings.h"

#pragma comment(lib, "dxgi.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics(HWND hWnd, size_t width, size_t height) 
	: width(width), height(height)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0;
#ifdef _DEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGIAdapter1* pAdapter;
	std::vector<IDXGIAdapter1*> vAdapters;
	IDXGIFactory1* pFactory = NULL;

	GFX_EXCEPT_INFO(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pFactory)));

	for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		vAdapters.push_back(pAdapter);
	}

	pFactory->Release();

	size_t adapter = vAdapters.size() - 2;
	if (adapter < 0) adapter = 0;

	GFX_EXCEPT_INFO(D3D11CreateDeviceAndSwapChain(
		vAdapters[0],
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	/*DXGI_ADAPTER_DESC desc{};
	vAdapters[adapter]->GetDesc(&desc);*/

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_EXCEPT_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_EXCEPT_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));


	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_EXCEPT_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_EXCEPT_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	GFX_EXCEPT_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV));

	pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), pDSV.Get());

	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

	DirectX::XMMatrixPerspectiveLH(1.0f, (float)height / (float)width, 0.5f, 200.0f);

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

Graphics::~Graphics() {
	ImGui_ImplDX11_Shutdown();
}

#ifdef _DEBUG
DxgiInfoManager& Graphics::getInfoManager() {
	return infoManager;
}
#endif

void Graphics::drawIndexed(UINT count) noexcept
{
	pContext->DrawIndexed(count, 0, 0);
}

void Graphics::setProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::getProjection() const noexcept
{
	return projection;
}

void Graphics::endFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	HRESULT hr;
	if (FAILED(hr = pSwap->Present(1, 0))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			GFX_EXCEPT_THROW(pDevice->GetDeviceRemovedReason());
		}
		else {
			GFX_EXCEPT_THROW(hr);
		}
	}
	if (temp_viewport) {
		if (width == 0 || height == 0) return;
		D3D11_VIEWPORT vp;
		vp.Width = (float) width;
		vp.Height = (float) height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		pContext->RSSetViewports(1u, &vp);
		projection = DirectX::XMMatrixPerspectiveLH(1.0f, vp.Height / vp.Width, 0.5f, 200.0f);
		temp_viewport = false;
	}
	pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), pDSV.Get());
}

void Graphics::setResoultion(int width, int height) noexcept
{
	this->width = width;
	this->height = height;
	temp_viewport = true;
}

void Graphics::beginFrame(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Graphics::setCamera(DirectX::FXMMATRIX cam) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::getCamera() const noexcept
{
	return camera;
}
