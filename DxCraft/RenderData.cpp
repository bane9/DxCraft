#include "RenderData.h"
#include "Surface.h"

void RenderData::CreateVertexShader(const wchar_t* filePath)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, pBytecodeBlob.ReleaseAndGetAddressOf());
	GFX_EXCEPT_INFO(gfx.pDevice->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		pVertexShader.ReleaseAndGetAddressOf()));
}

void RenderData::CreatePixelShader(const wchar_t* filePath)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, pBytecodeBlob.ReleaseAndGetAddressOf());
	GFX_EXCEPT_INFO(gfx.pDevice->CreatePixelShader(
		pBytecodeBlob.Get()->GetBufferPointer(),
		pBytecodeBlob.Get()->GetBufferSize(),
		nullptr,
		pPixelShader.ReleaseAndGetAddressOf()));

}

void RenderData::CreateGeometryShader(const wchar_t* filePath)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, pBytecodeBlob.ReleaseAndGetAddressOf());
	GFX_EXCEPT_INFO(gfx.pDevice->CreateGeometryShader(
		pBytecodeBlob.Get()->GetBufferPointer(),
		pBytecodeBlob.Get()->GetBufferSize(),
		nullptr,
		pGeometryShader.ReleaseAndGetAddressOf()));
}

void RenderData::Create2DTexture(const char* filePath)
{
	INFOMAN(gfx);
	Surface s = Surface::FromFile(filePath);

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

	GFX_EXCEPT_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd1, pTexture.ReleaseAndGetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	GFX_EXCEPT_INFO(gfx.pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, pTextureView.ReleaseAndGetAddressOf()));

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;


	GFX_EXCEPT_INFO(gfx.pDevice->CreateSamplerState(&samplerDesc, pSampler.ReleaseAndGetAddressOf()));
}

void RenderData::Render()
{
	RenderData::Render(*this, pVertexBuffer, pIndexBuffer, indexBufferSize, stride);
}

void RenderData::Render(const RenderData& data, const Microsoft::WRL::ComPtr<ID3D11Buffer> optVertexBuffer,
	Microsoft::WRL::ComPtr<ID3D11Buffer> optIndexBuffer, UINT indexBufferSize, UINT stride)
{
	if (indexBufferSize == 0) return;

	if (optVertexBuffer.Get() != nullptr)
		data.gfx.pContext->IASetVertexBuffers(0, 1, optVertexBuffer.GetAddressOf(), stride != 0 ? &stride : &data.stride, &data.offset);

	if (optIndexBuffer.Get() != nullptr)
		data.gfx.pContext->IASetIndexBuffer(optIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	if (data.pVertexShader.Get() != nullptr)
		data.gfx.pContext->VSSetShader(data.pVertexShader.Get(), nullptr, 0);

	if (data.pPixelShader.Get() != nullptr)
		data.gfx.pContext->PSSetShader(data.pPixelShader.Get(), nullptr, 0);

	if (data.pGeometryShader.Get() != nullptr)
		data.gfx.pContext->GSSetShader(data.pGeometryShader.Get(), nullptr, 0);

	if (data.pVertexInputLayout.Get() != nullptr)
		data.gfx.pContext->IASetInputLayout(data.pVertexInputLayout.Get());

	data.gfx.pContext->IASetPrimitiveTopology(data.topology);

	if (data.pTextureView.Get() != nullptr && data.pSampler.Get() != nullptr) {
		data.gfx.pContext->PSSetShaderResources(0, 1, data.pTextureView.GetAddressOf());
		data.gfx.pContext->PSSetSamplers(0, 1, data.pSampler.GetAddressOf());
	}

	if (data.pVertexCBuff.Get() != nullptr)
		data.gfx.pContext->VSSetConstantBuffers(0, 1, data.pVertexCBuff.GetAddressOf());

	if (data.pPixelCBuff.Get() != nullptr)
		data.gfx.pContext->PSSetConstantBuffers(0, 1, data.pPixelCBuff.GetAddressOf());

	if (data.pGeometryCBuff.Get() != nullptr)
		data.gfx.pContext->GSSetConstantBuffers(0, 1, data.pGeometryCBuff.GetAddressOf());

	switch (data.topology) {
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	default:
		data.gfx.pContext->DrawIndexed(indexBufferSize != 0 ? indexBufferSize : data.indexBufferSize, 0, 0);
		break;
	}
}

