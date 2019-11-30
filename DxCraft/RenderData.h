#pragma once
#include <d3d11.h>
#include <wrl.h>

struct RenderData {
	friend class RenderDataFactory;
	friend class Renderer;
public:
	D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pVertexInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexCBuff;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pPixelCBuff;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pGeometryCBuff;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelInputLayout;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;

	UINT offset = 0;
	UINT stride = 0;
	
	UINT indexBufferSize = 0;
};
