#pragma once
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>
#include "Graphics.h"
#include "ExceptionMacros.h"
#include "DxgiInfoManager.h"
#include "RenderData.h"

class RenderDataFactory
{
	RenderDataFactory() = delete;
	RenderDataFactory(const RenderDataFactory&) = delete;
	RenderDataFactory& operator=(const RenderDataFactory&) = delete;
public:

	template<typename Container>
	static void CreateVertexBuffer(Graphics& gfx, RenderData& data, const Container ct);

	template<typename Container>
	static void CreateIndexBuffer(Graphics& gfx, RenderData& data, const Container ct);

	template<typename Container>
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(Graphics& gfx, const Container ct);

	template<typename Container>
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(Graphics& gfx, const Container ct);

	template<typename Container>
	static void CreateVertexShader(Graphics& gfx, RenderData& data, const wchar_t* filePath, const Container& ied);

	static void CreatePixelShader(Graphics& gfx, RenderData& data, const wchar_t* filePath);

	static void CreateFSBlob(Graphics& gfx, RenderData& data, const wchar_t* filePath);

	static void CreateGSBlob(Graphics& gfx, RenderData& data, const wchar_t* filePath);

	static void Create2DTexture(Graphics& gfx, RenderData& data, const char* filePath);

	template<typename T>
	static void UpdateVScBuf(Graphics& gfx, RenderData& data, T cBuf);
};

template<typename Container>
inline void RenderDataFactory::CreateVertexBuffer(Graphics& gfx, RenderData& data, const Container ct)
{
	assert(ct.size() > 0 && "Vertex buffer is empty");
	INFOMAN(gfx);
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	auto asd = sizeof(Container::value_type);
	bd.ByteWidth = static_cast<UINT>(ct.size() * sizeof(Container::value_type));
	bd.StructureByteStride = sizeof(Container::value_type);
	data.stride = sizeof(Container::value_type);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = ct.data();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &data.pVertexBuffer));

}

template<typename Container>
inline void RenderDataFactory::CreateIndexBuffer(Graphics& gfx, RenderData& data, const Container ct)
{
	assert(ct.size() > 0 && "Index buffer is empty");
	INFOMAN(gfx);
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = static_cast<UINT>(ct.size() * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = ct.data();
	data.indexBufferSize = ct.size();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&ibd, &isd, &data.pIndexBuffer));
}

template<typename Container>
inline Microsoft::WRL::ComPtr<ID3D11Buffer> RenderDataFactory::CreateVertexBuffer(Graphics& gfx, const Container ct)
{
	assert(ct.size() > 0 && "Vertex buffer is empty");
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;

	INFOMAN(gfx);
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	auto asd = sizeof(Container::value_type);
	bd.ByteWidth = static_cast<UINT>(ct.size() * sizeof(Container::value_type));
	bd.StructureByteStride = sizeof(Container::value_type);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = ct.data();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	return pVertexBuffer;
}

template<typename Container>
inline Microsoft::WRL::ComPtr<ID3D11Buffer> RenderDataFactory::CreateIndexBuffer(Graphics& gfx, const Container ct)
{
	assert(ct.size() > 0 && "Index buffer is empty");
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;

	INFOMAN(gfx);
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = static_cast<UINT>(ct.size() * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = ct.data();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	return pIndexBuffer;
}

template<typename Container>
inline void RenderDataFactory::CreateVertexShader(Graphics& gfx, RenderData& data, const wchar_t* filePath, const Container& ied)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, pBytecodeBlob.ReleaseAndGetAddressOf());
	GFX_EXCEPT_INFO(gfx.pDevice->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		data.pVertexShader.GetAddressOf()));

	GFX_EXCEPT_INFO(gfx.pDevice->CreateInputLayout(
		ied.data(),
		ied.size(),
		pBytecodeBlob.Get()->GetBufferPointer(),
		pBytecodeBlob.Get()->GetBufferSize(),
		data.pVertexInputLayout.GetAddressOf()));
}

template<typename T>
inline void RenderDataFactory::UpdateVScBuf(Graphics& gfx, RenderData& data, T cBuf)
{
	INFOMAN(gfx);
	if (data.pConstantBuffer.Get() == nullptr) {
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0;
		cbd.ByteWidth = sizeof(cBuf);
		cbd.StructureByteStride = 0;

		GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&cbd, nullptr, data.pConstantBuffer.ReleaseAndGetAddressOf()));
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	gfx.pContext->Map(data.pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, &cBuf, sizeof(cBuf));
	gfx.pContext->Unmap(data.pConstantBuffer.Get(), 0);
}
