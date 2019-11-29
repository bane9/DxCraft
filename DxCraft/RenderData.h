#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <tuple>
#include <utility>

struct RenderData {
	void AppendVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer) {
		this->pVertexBuffer = pVertexBuffer;
	}

	void AppendIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer) {
		this->pIndexBuffer = pIndexBuffer;
	}

	void Append(std::pair<Microsoft::WRL::ComPtr<ID3D11VertexShader>, Microsoft::WRL::ComPtr<ID3D11InputLayout>> data) {
		pVertexShader = data.first;
		pVertexInputLayout = data.second;
	}

	void Append(Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader) {
		this->pPixelShader = pPixelShader;
	}

	void Append(std::tuple<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, Microsoft::WRL::ComPtr<ID3D11Texture2D>,
		Microsoft::WRL::ComPtr<ID3D11SamplerState>> data) {
		pTextureView = std::get<0>(data);
		pTexture = std::get<1>(data);
		pSampler = std::get<2>(data);
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pVertexInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;

	UINT offset = 0;
	UINT stride = 0;
	
	UINT indexBufferSize = 0;

	D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
