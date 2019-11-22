#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Surface.h"
#include <vector>
#include "Graphics.h"
#include "ExceptionMacros.h"
#include "DxgiInfoManager.h"
#include <optional>
#include <utility>

template<typename vertexType, typename cBuf>
class RendererData
{
	friend class Renderer;
public:
	RendererData(
		Graphics& gfx,
		const wchar_t* vertexShader,
		const wchar_t* pixelShader,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputDescriptor,
		const char* texture = nullptr
	);

	RendererData(const RendererData&) = delete;
	RendererData& operator=(const RendererData&) = delete;

	std::optional<std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, size_t>> UpdateVertexBuffer(std::vector<vertexType>& vertexBuffer);
	std::optional<std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, size_t>> UpdateIndexBuffer(std::vector<uint16_t>& indexBuffer);
	void UpdateConstantBuffer(const cBuf& constBuffer);

	void CopyVertexPtr(Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer, size_t size);
	void CopyIndexPtr(Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer, size_t size);

	void Reset();

private:
	Graphics& gfx;

	bool hasTexture = false;

	size_t vertexBufferSize = 0;
	size_t indexBufferSize = 0;

#ifdef _DEBUG
	DxgiInfoManager& infoManager;
#endif 

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
};

template<typename vertexType, typename cBuf>
inline RendererData<vertexType, cBuf>::RendererData(Graphics& gfx, const wchar_t* vertexShader, const wchar_t* pixelShader, 
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputDescriptor, const char* texture)
	: gfx(gfx) 
#ifdef _DEBUG
	, infoManager(gfx.getInfoManager())
#endif
{
	if (texture != nullptr) {
		Surface s = Surface::FromFile(texture);

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
		GFX_EXCEPT_INFO(gfx.getDevice()->CreateTexture2D(&textureDesc, &sd1, &pTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_EXCEPT_INFO(gfx.getDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		GFX_EXCEPT_INFO(gfx.getDevice()->CreateSamplerState(&samplerDesc, &pSampler));

		hasTexture = true;
	}

	D3DReadFileToBlob(vertexShader, &pBytecodeBlob);
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateVertexShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, &pVertexShader));

	auto blobVS = pBytecodeBlob.Get();

	D3DReadFileToBlob(pixelShader, &pBlob);
	GFX_EXCEPT_INFO(gfx.getDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	GFX_EXCEPT_INFO(gfx.getDevice()->CreateInputLayout(inputDescriptor.data(), 
		static_cast<UINT>(inputDescriptor.size()), blobVS->GetBufferPointer(), blobVS->GetBufferSize(), &pInputLayout));

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(cBuf);
	cbd.StructureByteStride = 0;

	GFX_EXCEPT_INFO(gfx.getDevice()->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
}

template<typename vertexType, typename cBuf>
inline std::optional<std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, size_t>>
	RendererData<vertexType, cBuf>::UpdateVertexBuffer(std::vector<vertexType>& vertexBuffer)
{
	if (vertexBuffer.size() == 0) return {};
	vertexBufferSize = vertexBuffer.size();
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = static_cast<UINT>(vertexBufferSize * sizeof(vertexType));
	bd.StructureByteStride = sizeof(vertexType);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertexBuffer.data();
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));
	
	return std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, size_t>{ pVertexBuffer, vertexBufferSize };
}

template<typename vertexType, typename cBuf>
inline std::optional<std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, size_t>>
	RendererData<vertexType, cBuf>::UpdateIndexBuffer(std::vector<uint16_t>& indexBuffer)
{
	if (indexBuffer.size() == 0) return {};
	indexBufferSize = indexBuffer.size();
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = static_cast<UINT>(indexBufferSize * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indexBuffer.data();
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateBuffer(&ibd, &isd, &pIndexBuffer));
	
	return std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, size_t>{ pIndexBuffer, indexBufferSize };
}

template<typename vertexType, typename cBuf>
inline void RendererData<vertexType, cBuf>::UpdateConstantBuffer(const cBuf& constBuffer)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	gfx.pContext->Map(pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, &constBuffer, sizeof(cBuf));
	gfx.pContext->Unmap(pConstantBuffer.Get(), 0);
}

template<typename vertexType, typename cBuf>
inline void RendererData<vertexType, cBuf>::CopyVertexPtr(Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer, size_t size)
{
	if (size == 0) {
		Reset();
		return;
	}

	this->pVertexBuffer = pVertexBuffer;
	vertexBufferSize = size;
}

template<typename vertexType, typename cBuf>
inline void RendererData<vertexType, cBuf>::CopyIndexPtr(Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer, size_t size)
{
	if (size == 0) {
		Reset();
		return;
	}

	this->pIndexBuffer = pIndexBuffer;
	indexBufferSize = size;
}

template<typename vertexType, typename cBuf>
inline void RendererData<vertexType, cBuf>::Reset()
{
	vertexBufferSize = 0;
	indexBufferSize = 0;
	pVertexBuffer.Detach();
	pIndexBuffer.Detach();
}
