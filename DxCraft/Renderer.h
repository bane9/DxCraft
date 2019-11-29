#pragma once
#include "Graphics.h"
#include <wrl.h>
#include <d3d11.h>
#include "RenderData.h"

class Renderer {
	Renderer() = delete;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
public:
	static void DrawIndexed(Graphics& gfx, const RenderData& data, Microsoft::WRL::ComPtr<ID3D11Buffer> optVertexBuffer = nullptr,
		Microsoft::WRL::ComPtr<ID3D11Buffer> optIndexBuffer = nullptr, UINT indexBufferSize = 0, UINT stride = 0) {

		if ((indexBufferSize != 0 ? indexBufferSize : data.indexBufferSize) == 0) return;

		if(optVertexBuffer.Get() != nullptr)
			gfx.pContext->IASetVertexBuffers(0, 1, optVertexBuffer.GetAddressOf(), stride != 0 ? &stride : &data.stride, &data.offset);
		else
			gfx.pContext->IASetVertexBuffers(0, 1, data.pVertexBuffer.GetAddressOf(), stride != 0 ? &stride : &data.stride, &data.offset);

		if (optIndexBuffer.Get() != nullptr)
			gfx.pContext->IASetIndexBuffer(optIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		else
			gfx.pContext->IASetIndexBuffer(data.pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		

		if(data.pVertexShader.Get() != nullptr)
			gfx.pContext->VSSetShader(data.pVertexShader.Get(), nullptr, 0);

		if (data.pPixelShader.Get() != nullptr)
			gfx.pContext->PSSetShader(data.pPixelShader.Get(), nullptr, 0);

		if(data.pVertexInputLayout.Get() != nullptr)
			gfx.pContext->IASetInputLayout(data.pVertexInputLayout.Get());

		gfx.pContext->IASetPrimitiveTopology(data.topology);

		if (data.pTextureView.Get() != nullptr && data.pSampler.Get() != nullptr) {
			gfx.pContext->PSSetShaderResources(0, 1, data.pTextureView.GetAddressOf());
			gfx.pContext->PSSetSamplers(0, 1, data.pSampler.GetAddressOf());
		}

		if (data.pConstantBuffer.Get() != nullptr)
			gfx.pContext->VSSetConstantBuffers(0, 1, data.pConstantBuffer.GetAddressOf());

		
		gfx.pContext->DrawIndexed(indexBufferSize != 0 ? indexBufferSize : data.indexBufferSize, 0, 0);
	}
};
