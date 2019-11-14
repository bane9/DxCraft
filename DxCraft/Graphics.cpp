#include "Graphics.h"
#include "ExceptionMacros.h"
#include "DxgiInfoManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "Surface.h"
#pragma comment(lib, "dxgi.lib")

#define FLIPEFFECT

#ifdef FLIPEFFECT
#define EFFECT DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
#define BUFFERCOUNT 2
#else
#define EFFECT DXGI_SWAP_EFFECT_DISCARD;
#define BUFFERCOUNT 1
#endif

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics(HWND hWnd, size_t width, size_t height) 
	:width(width), height(height)
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
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;

	// create device and front/back buffers, and swap chain and rendering context
	GFX_EXCEPT_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
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

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_EXCEPT_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_EXCEPT_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_EXCEPT_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	// bind depth state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// create depth stensil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_EXCEPT_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_EXCEPT_INFO(pDevice->CreateDepthStencilView(
		pDepthStencil.Get(), &descDSV, &pDSV
	));

	// bind depth stensil view to OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

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
#ifdef FLIPEFFECT
	pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), pDSV.Get());
#endif
}

void Graphics::Transform(std::vector<Vertex>& vertices)
{
	
}

void Graphics::DrawTestTriangle(float angle, float x, float z)
{
	namespace dx = DirectX;

	float side = 1.0f;
	
	std::vector<Vertex> vertices(14);

	vertices[0].pos = { -side,-side,-side };
	vertices[0].tex = { 2.0f / 3.0f,0.0f / 4.0f };
	vertices[1].pos = { side,-side,-side };
	vertices[1].tex = { 1.0f / 3.0f,0.0f / 4.0f };
	vertices[2].pos = { -side,side,-side };
	vertices[2].tex = { 2.0f / 3.0f,1.0f / 4.0f };
	vertices[3].pos = { side,side,-side };
	vertices[3].tex = { 1.0f / 3.0f,1.0f / 4.0f };
	vertices[4].pos = { -side,-side,side };
	vertices[4].tex = { 2.0f / 3.0f,3.0f / 4.0f };
	vertices[5].pos = { side,-side,side };
	vertices[5].tex = { 1.0f / 3.0f,3.0f / 4.0f };
	vertices[6].pos = { -side,side,side };
	vertices[6].tex = { 2.0f / 3.0f,2.0f / 4.0f };
	vertices[7].pos = { side,side,side };
	vertices[7].tex = { 1.0f / 3.0f,2.0f / 4.0f };
	vertices[8].pos = { -side,-side,-side };
	vertices[8].tex = { 2.0f / 3.0f,4.0f / 4.0f };
	vertices[9].pos = { side,-side,-side };
	vertices[9].tex = { 1.0f / 3.0f,4.0f / 4.0f };
	vertices[10].pos = { -side,-side,-side };
	vertices[10].tex = { 3.0f / 3.0f,1.0f / 4.0f };
	vertices[11].pos = { -side,-side,side };
	vertices[11].tex = { 3.0f / 3.0f,2.0f / 4.0f };
	vertices[12].pos = { side,-side,-side };
	vertices[12].tex = { 0.0f / 3.0f,1.0f / 4.0f };
	vertices[13].pos = { side,-side,side };
	vertices[13].tex = { 0.0f / 3.0f,2.0f / 4.0f };

	std::vector<unsigned short> indices = {
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
	};

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = UINT(vertices.size() * sizeof(Vertex));
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices.data();
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	
	GFX_EXCEPT_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	const UINT offset = 0u;
	UINT stride = sizeof(Vertex);
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;


	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = s.GetWidth();
	textureDesc.Height = s.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd1 = {};
	sd1.pSysMem = s.GetBufferPtr();
	sd1.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);
	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_EXCEPT_INFO(pDevice->CreateTexture2D(
		&textureDesc, &sd1, &pTexture
	));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_EXCEPT_INFO(pDevice->CreateShaderResourceView(
		pTexture.Get(), &srvDesc, &pTextureView
	));

	pContext->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;

	GFX_EXCEPT_INFO(pDevice->CreateSamplerState(&samplerDesc, &pSampler));
	pContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());

	GFX_EXCEPT_INFO(D3DReadFileToBlob(L"TextureVS.cso", &pBytecodeBlob));
	GFX_EXCEPT_INFO(pDevice->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	));

	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	auto blobVS = pBytecodeBlob.Get();

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_EXCEPT_INFO(D3DReadFileToBlob(L"TexturePS.cso", &pBlob));
	GFX_EXCEPT_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	UINT count = indices.size();
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(count * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	GFX_EXCEPT_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

	GFX_EXCEPT_INFO(pDevice->CreateInputLayout(
		ied.data(), (UINT)ied.size(),
		blobVS->GetBufferPointer(),
		blobVS->GetBufferSize(),
		&pInputLayout
	));

	pContext->IASetInputLayout(pInputLayout.Get());

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto model = dx::XMMatrixTranslation(1.0f, 1.0f, 6.0f);

	struct Transforms
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};

	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(model * camera * projection),
		DirectX::XMMatrixTranspose(model)
		
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(tf);
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &tf;
	GFX_EXCEPT_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	D3D11_MAPPED_SUBRESOURCE msr;
	GFX_EXCEPT_INFO(pContext->Map(
		pConstantBuffer.Get(), 0u,
		D3D11_MAP_WRITE_DISCARD, 0u,
		&msr
	));
	memcpy(msr.pData, &tf, sizeof(tf));
	pContext->Unmap(pConstantBuffer.Get(), 0u);
	pContext->VSSetConstantBuffers(0, 1u, pConstantBuffer.GetAddressOf());

	pContext->DrawIndexed(std::size(indices), 0u, 0u);
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