#pragma once
#include "Graphics.h"
#include "ExceptionMacros.h"
#include <vector>
#include "Surface.h"
#include "Block.h"


struct indvVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 n;
	DirectX::XMFLOAT2 tc;
};

class MeshRenderer
{
public:
	MeshRenderer(Graphics& gfx);
	MeshRenderer(MeshRenderer&) = delete;
	MeshRenderer& operator=(MeshRenderer&) = delete;

	void Draw(Block& block);

private:
	void AppendFace(std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> face);
	std::vector<unsigned short> indices;
	std::vector<indvVertex> vertices;
	const float side = 1.0f;
	Graphics& gfx;

	const UINT offset = 0u;
	UINT stride = sizeof(indvVertex);

	Surface s = Surface::FromFile("images\\block_face.png");

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
};

