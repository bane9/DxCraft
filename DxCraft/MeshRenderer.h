#pragma once
#include "Graphics.h"
#include "ExceptionMacros.h"
#include <vector>
#include "Surface.h"
#include "Block.h"


class MeshRenderer
{
public:
	MeshRenderer(Graphics& gfx);
	MeshRenderer(MeshRenderer&) = delete;
	MeshRenderer& operator=(MeshRenderer&) = delete;

	void Draw(Block& block);

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
	const float side = 1.0f;
	Graphics& gfx;

	const UINT offset = 0u;
	UINT stride = sizeof(Vertex);

	Surface s = Surface::FromFile("images\\cube.png");

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

