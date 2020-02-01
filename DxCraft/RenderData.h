#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Graphics.h"
#include "ExceptionMacros.h"
#include "Cbuffer.h"
#include <vector>

class RenderData {
public:
	RenderData(Graphics& gfx) : gfx(gfx) {}
	~RenderData() = default;

	template<typename Container>
	void CreateVertexBuffer(const Container& ct);

	template<typename Container>
	void CreateIndexBuffer(const Container& ct);

	template<typename Container>
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(Graphics& gfx, const Container& ct);

	template<typename Container>
	static Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(Graphics& gfx, const Container& ct);

	void CreateVertexShader(const wchar_t* filePath);

	template<typename Container>
	void CreateVertexShader(const wchar_t* filePath, const Container& ied);

	void CreatePixelShader(const wchar_t* filePath);

	void CreateGeometryShader(const wchar_t* filePath);

	void Create2DTexture(const char* filePath);

	template<typename T>
	void UpdateVScBuf(const T& cBuf, int slot = 0);

	template<typename T>
	void UpdatePScBuf(const T& cBuf, int slot = 0);

	template<typename T>
	void UpdateGScBuf(const T& cBuf, int slot = 0);

	static void Render(const RenderData& data, const Microsoft::WRL::ComPtr<ID3D11Buffer> optVertexBuffer,
		Microsoft::WRL::ComPtr<ID3D11Buffer> optIndexBuffer, UINT indexBufferSize, UINT stride);

	void Render();

	D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pVertexInputLayout;
	std::vector<Cbuffer> VertexCbuffers{ D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT };
	std::vector<Cbuffer> PixelCbuffers{ D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT };
	std::vector<Cbuffer> GeomteryCbuffers{ D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT };
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelInputLayout;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;

	UINT offset = 0;
	UINT stride = 0;
	
	UINT indexBufferSize = 0;

	Graphics& gfx;
};

template<typename Container>
inline void RenderData::CreateVertexBuffer(const Container& ct)
{
	assert(ct.size() > 0 && "Vertex buffer is empty");
	INFOMAN(gfx);
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = static_cast<UINT>(ct.size() * sizeof(Container::value_type));
	bd.StructureByteStride = sizeof(Container::value_type);
	stride = sizeof(Container::value_type);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = ct.data();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

}

template<typename Container>
inline void RenderData::CreateIndexBuffer(const Container& ct)
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
	indexBufferSize = ct.size();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

template<typename Container>
static inline Microsoft::WRL::ComPtr<ID3D11Buffer> RenderData::CreateVertexBuffer(Graphics& gfx, const Container& ct)
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
static inline Microsoft::WRL::ComPtr<ID3D11Buffer> RenderData::CreateIndexBuffer(Graphics& gfx, const Container& ct)
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
inline void RenderData::CreateVertexShader(const wchar_t* filePath, const Container& ied)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, pBytecodeBlob.ReleaseAndGetAddressOf());
	GFX_EXCEPT_INFO(gfx.pDevice->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		pVertexShader.ReleaseAndGetAddressOf()));

	GFX_EXCEPT_INFO(gfx.pDevice->CreateInputLayout(
		ied.data(),
		ied.size(),
		pBytecodeBlob.Get()->GetBufferPointer(),
		pBytecodeBlob.Get()->GetBufferSize(),
		pVertexInputLayout.ReleaseAndGetAddressOf()));
}

template<typename T>
inline void RenderData::UpdateVScBuf(const T& cBuf, int slot)
{
	VertexCbuffers[slot].Update(cBuf, gfx);
}

template<typename T>
inline void RenderData::UpdatePScBuf(const T& cBuf, int slot)
{
	PixelCbuffers[slot].Update(cBuf, gfx);
}

template<typename T>
inline void RenderData::UpdateGScBuf(const T& cBuf, int slot)
{
	GeomteryCbuffers[slot].Update(cBuf, gfx);
}
