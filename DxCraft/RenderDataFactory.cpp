#include "RenderDataFactory.h"
#include "Surface.h"

void RenderDataFactory::CreatePixelShader(Graphics& gfx, RenderData& data, const wchar_t* filePath)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, pBytecodeBlob.ReleaseAndGetAddressOf());
	GFX_EXCEPT_INFO(gfx.pDevice->CreatePixelShader(
		pBytecodeBlob.Get()->GetBufferPointer(),
		pBytecodeBlob.Get()->GetBufferSize(), 
		nullptr, 
		data.pPixelShader.GetAddressOf()));

}

void RenderDataFactory::CreateFragmentShader(Graphics& gfx, RenderData& data, const wchar_t* filePath)
{

}

void RenderDataFactory::CreateGeometryShader(Graphics& gfx, RenderData& data, const wchar_t* filePath)
{

}

void RenderDataFactory::Create2DTexture(Graphics& gfx, RenderData& data, const char* filePath)
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

	GFX_EXCEPT_INFO(gfx.pDevice->CreateTexture2D(&textureDesc, &sd1, data.pTexture.ReleaseAndGetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	GFX_EXCEPT_INFO(gfx.pDevice->CreateShaderResourceView(data.pTexture.Get(), &srvDesc, data.pTextureView.ReleaseAndGetAddressOf()));

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;


	GFX_EXCEPT_INFO(gfx.pDevice->CreateSamplerState(&samplerDesc, data.pSampler.ReleaseAndGetAddressOf()));
}
