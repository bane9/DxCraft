#include "MeshRenderer.h"
#include "ExceptionMacros.h"
#include <array>
#include <utility>
#include <algorithm>

MeshRenderer::MeshRenderer(Graphics& gfx)
	: gfx(gfx)
#ifdef _DEBUG
	, infoManager(gfx.infoManager)
#endif
	
{
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
	GFX_EXCEPT_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd1, &pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_EXCEPT_INFO(gfx.pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_EXCEPT_INFO(gfx.pDevice->CreateSamplerState(&samplerDesc, &pSampler));

	D3DReadFileToBlob(L"TextureVS.cso", &pBytecodeBlob);
	GFX_EXCEPT_INFO(gfx.pDevice->CreateVertexShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, &pVertexShader));

	auto blobVS = pBytecodeBlob.Get();

	D3DReadFileToBlob(L"TexturePS.cso", &pBlob);
	GFX_EXCEPT_INFO(gfx.pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	GFX_EXCEPT_INFO(gfx.pDevice->CreateInputLayout(ied.data(), static_cast<UINT>(ied.size()), blobVS->GetBufferPointer(), blobVS->GetBufferSize(), &pInputLayout));

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(Transforms);
	cbd.StructureByteStride = 0;

	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
}

void MeshRenderer::Draw(const BasicChunk& chunk) {

	if (chunk.vertexBufferSize == 0 || chunk.indexBufferSize == 0) return;

		gfx.pContext->IASetVertexBuffers(0, 1, chunk.pVertexBuffer.GetAddressOf(), &stride, &offset);

		gfx.pContext->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());

		gfx.pContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());

		gfx.pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

		gfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

		gfx.pContext->IASetIndexBuffer(chunk.pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		gfx.pContext->IASetInputLayout(pInputLayout.Get());

		gfx.pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto model = DirectX::XMMatrixTranslation(chunk.x, chunk.y, chunk.z);

		const Transforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.camera * gfx.projection),
			DirectX::XMMatrixTranspose(model)
		};

		D3D11_MAPPED_SUBRESOURCE msr;
		gfx.pContext->Map(pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, &tf, sizeof(tf));
		gfx.pContext->Unmap(pConstantBuffer.Get(), 0);

		gfx.pContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());

		gfx.pContext->DrawIndexed(chunk.indexBufferSize, 0, 0);
}

void MeshRenderer::AppendData(BasicChunk& chunk)
{
	if (chunk.vertices.empty() || chunk.indices.empty()) return;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = static_cast<UINT>(chunk.vertices.size() * sizeof(Vertex));
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = chunk.vertices.data();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));


	UINT count = chunk.indices.size();
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = static_cast<UINT>(count * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = chunk.indices.data();
	GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	chunk.pVertexBuffer = std::move(pVertexBuffer);
	chunk.pIndexBuffer = std::move(pIndexBuffer);

	chunk.vertexBufferSize = chunk.vertices.size();
	chunk.indexBufferSize =  chunk.indices.size();

	chunk.vertices.clear();
	chunk.indices.clear();
}
