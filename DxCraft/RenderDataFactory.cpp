#include "RenderDataFactory.h"
#include "Surface.h"

std::pair<Microsoft::WRL::ComPtr<ID3D11VertexShader>, Microsoft::WRL::ComPtr<ID3D11InputLayout>>
	RenderDataFactory::CreateVSBlob(Graphics& gfx, const wchar_t* filePath, D3D11_INPUT_ELEMENT_DESC* ied, UINT ied_size)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, &pBytecodeBlob);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(), 
		pBytecodeBlob->GetBufferSize(), 
		nullptr, 
		&pVertexShader));

	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateInputLayout(ied,
		ied_size, pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), &pInputLayout));

	return {pVertexShader, pInputLayout};
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> RenderDataFactory::CreatePSBlob(Graphics& gfx, const wchar_t* filePath)
{
	INFOMAN(gfx);
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	D3DReadFileToBlob(filePath, &pBytecodeBlob);
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreatePixelShader(pBytecodeBlob->GetBufferPointer(), 
		pBytecodeBlob->GetBufferSize(), nullptr, &pPixelShader));
	return pPixelShader;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderDataFactory::CreateFSBlob(Graphics& gfx, const wchar_t* filePath)
{
	return Microsoft::WRL::ComPtr<ID3D11Buffer>();
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RenderDataFactory::CreateGSBlob(Graphics& gfx, const wchar_t* filePath)
{
	return Microsoft::WRL::ComPtr<ID3D11Buffer>();
}

std::tuple<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 
	Microsoft::WRL::ComPtr<ID3D11Texture2D>, 
	Microsoft::WRL::ComPtr<ID3D11SamplerState>> 
	RenderDataFactory::Create2DTexture(Graphics& gfx, const char* filePath)
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
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateTexture2D(&textureDesc, &sd1, &pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	GFX_EXCEPT_INFO(gfx.getDevice()->CreateSamplerState(&samplerDesc, &pSampler));

	return { pTextureView, pTexture, pSampler };

}
