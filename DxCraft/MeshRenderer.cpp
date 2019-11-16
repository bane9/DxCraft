#include "MeshRenderer.h"
#include <array>
#include <utility>
#include <algorithm>

#pragma warning(disable : 26812)

struct Transforms
{
	DirectX::XMMATRIX modelViewProj;
	DirectX::XMMATRIX model;
};

struct Faces {
	static constexpr float side = 2.0f;
	static constexpr std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> NearSide{
		{
			indvVertex{DirectX::XMFLOAT3(-side,-side,-side), DirectX::XMFLOAT3(), { 0.0f,0.0f }},
			indvVertex{DirectX::XMFLOAT3(side,-side,-side),  DirectX::XMFLOAT3(), { 1.0f,0.0f }},
			indvVertex{DirectX::XMFLOAT3(-side,side,-side),  DirectX::XMFLOAT3(), { 0.0f,1.0f }},
			indvVertex{DirectX::XMFLOAT3(side,side,-side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 2, 1, 2, 3, 1}
	};

	static constexpr std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> FarSide{
		{
		indvVertex{DirectX::XMFLOAT3(-side,-side,side),  DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(side,-side,side),   DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(-side,side,side),   DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		indvVertex{DirectX::XMFLOAT3(side,side,side) ,	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 1, 3, 0, 3, 2}
	};					

	static constexpr std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> LeftSide{
		{
		indvVertex{DirectX::XMFLOAT3(-side,-side,-side), DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(-side,side,-side),  DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		indvVertex{DirectX::XMFLOAT3(-side,-side,side),  DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(-side,side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 2, 1, 2, 3, 1}
	};		

	static constexpr std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> RightSide{
		{
		indvVertex{DirectX::XMFLOAT3(side,-side,-side),  DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(side,side,-side),   DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		indvVertex{DirectX::XMFLOAT3(side,-side,side),   DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(side,side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 1, 3, 0, 3, 2}
	};

	static constexpr std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> BottomSide{
		{
		indvVertex{DirectX::XMFLOAT3(-side,-side,-side), DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(side,-side,-side),  DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(-side,-side,side),  DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		indvVertex{DirectX::XMFLOAT3(side,-side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 1, 2, 2, 1, 3}
	};

	static constexpr std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> TopSide{
		{
		indvVertex{DirectX::XMFLOAT3(-side,side,-side),  DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(side,side,-side),   DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		indvVertex{DirectX::XMFLOAT3(-side,side,side),   DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		indvVertex{DirectX::XMFLOAT3(side,side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 3, 1, 0, 2, 3}
	};
};

void MeshRenderer::AppendFace(std::pair<std::array<indvVertex, 4>, std::array<uint16_t, 6>> face)
{
	std::copy(face.first.begin(), face.first.end(), std::back_inserter(vertices));
	int offset = (vertices.size() / 4 - 1) * 4;
	indices.push_back(offset + face.second[0]);
	indices.push_back(offset + face.second[1]);
	indices.push_back(offset + face.second[2]);
	indices.push_back(offset + face.second[3]);
	indices.push_back(offset + face.second[4]);
	indices.push_back(offset + face.second[5]);
}

MeshRenderer::MeshRenderer(Graphics& gfx)
	: gfx(gfx)
{
	AppendFace(Faces::NearSide);
	AppendFace(Faces::FarSide);
	//AppendFace(Faces::LeftSide);
	AppendFace(Faces::RightSide);
	AppendFace(Faces::BottomSide);
	//AppendFace(Faces::TopSide);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = UINT(vertices.size() * sizeof(indvVertex));
	bd.StructureByteStride = sizeof(indvVertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices.data();

	gfx.pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);

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
	gfx.pDevice->CreateTexture2D(&textureDesc, &sd1, &pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	gfx.pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	gfx.pDevice->CreateSamplerState(&samplerDesc, &pSampler);

	D3DReadFileToBlob(L"TextureVS.cso", &pBytecodeBlob);
	gfx.pDevice->CreateVertexShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, &pVertexShader);

	auto blobVS = pBytecodeBlob.Get();

	D3DReadFileToBlob(L"TexturePS.cso", &pBlob);
	gfx.pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

	UINT count = indices.size();

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = UINT(count * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	gfx.pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	gfx.pDevice->CreateInputLayout(ied.data(), static_cast<UINT>(ied.size()), blobVS->GetBufferPointer(), blobVS->GetBufferSize(), &pInputLayout);

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(Transforms);
	cbd.StructureByteStride = 0;

	gfx.pDevice->CreateBuffer(&cbd, nullptr, &pConstantBuffer);
}

void MeshRenderer::Draw(Block& block) {
	if (block.type == BlockType::Air) return;
	gfx.pContext->IASetVertexBuffers(0u, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);

	gfx.pContext->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());

	gfx.pContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());

	gfx.pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

	gfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

	gfx.pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	gfx.pContext->IASetInputLayout(pInputLayout.Get());

	gfx.pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto model = DirectX::XMMatrixTranslation(block.x * 2.0f, block.y * 2.0f, block.z * 2.0f);

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

	gfx.pContext->DrawIndexed(indices.size(), 0, 0);
}
