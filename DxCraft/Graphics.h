#pragma once
#include "WinConfig.h"
#include <wrl.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include "CustomException.h"
#include "DxgiInfoManager.h"
#include "Surface.h"
#include <optional>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	struct
	{
		float u;
		float v;
	} tex;
};

class Graphics
{
	friend class Block;
public:
	Graphics(HWND hWnd, size_t width, size_t height);
	Graphics(Graphics&) = delete;
	~Graphics();
	Graphics operator=(Graphics&) = delete;
	void setCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX getCamera() const noexcept;
	void endFrame();
	void drawIndexed(UINT count) noexcept;
	void setProjection(DirectX::FXMMATRIX proj) noexcept;
	void beginFrame(float red, float green, float blue) noexcept;
	DirectX::XMMATRIX getProjection() const noexcept;
	void Transform(std::vector<Vertex>& vertices);
	void setResoultion(int x, int y) noexcept;
#ifdef _DEBUG
	DxgiInfoManager& getInfoManager();
#endif
private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	size_t width;
	size_t height;
	bool temp_viewport = false;
#ifdef _DEBUG
	DxgiInfoManager infoManager;
#endif
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
};