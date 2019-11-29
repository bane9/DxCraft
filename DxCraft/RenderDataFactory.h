#pragma once
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>
#include <utility>
#include <tuple>
#include "Graphics.h"
#include "ExceptionMacros.h"
#include "DxgiInfoManager.h"

class RenderDataFactory
{
	RenderDataFactory() = delete;
	RenderDataFactory(const RenderDataFactory&) = delete;
	RenderDataFactory& operator=(const RenderDataFactory&) = delete;
public:

	template<typename Container>
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(Graphics& gfx, const Container ct);

	template<typename Container>
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(Graphics& gfx, const Container ct);

	static std::pair<Microsoft::WRL::ComPtr<ID3D11VertexShader>, Microsoft::WRL::ComPtr<ID3D11InputLayout>>
		CreateVertexShader(Graphics& gfx, const wchar_t* filePath, const D3D11_INPUT_ELEMENT_DESC* ied, UINT ied_size);

	static Microsoft::WRL::ComPtr<ID3D11PixelShader> CreatePixelShader(Graphics& gfx, const wchar_t* filePath);

	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateFSBlob(Graphics& gfx, const wchar_t* filePath);

	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateGSBlob(Graphics& gfx, const wchar_t* filePath);

	static std::tuple<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, Microsoft::WRL::ComPtr<ID3D11Texture2D>,
		Microsoft::WRL::ComPtr<ID3D11SamplerState>>
		Create2DTexture(Graphics& gfx, const char* filePath);

	template<typename T>
	static void UpdateVScBuf(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Buffer>& ptr, T cBuf);
};

template<typename Container>
inline Microsoft::WRL::ComPtr<ID3D11Buffer>
	RenderDataFactory::CreateVertexBuffer(Graphics& gfx, const Container ct)
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
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = ct.data();
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));

	return pVertexBuffer;
}

template<typename Container>
inline Microsoft::WRL::ComPtr<ID3D11Buffer> RenderDataFactory::CreateIndexBuffer(Graphics& gfx, const Container ct)
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
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	return pIndexBuffer;
}

template<typename T>
inline void RenderDataFactory::UpdateVScBuf(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Buffer>& ptr, T cBuf)
{
	INFOMAN(gfx);
	auto asd = ptr.Get();
	if (ptr.Get() == nullptr) {
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0;
		cbd.ByteWidth = sizeof(cBuf);
		cbd.StructureByteStride = 0;

		GFX_EXCEPT_INFO(gfx.getDevice()->CreateBuffer(&cbd, nullptr, ptr.ReleaseAndGetAddressOf()));
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	gfx.getContext()->Map(ptr.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, &cBuf, sizeof(cBuf));
	gfx.getContext()->Unmap(ptr.Get(), 0);
}
