#pragma once
#include "RendererData.h"
#include "Graphics.h"
#include "ExceptionMacros.h"


class Renderer {
public:
	template<typename vertexType, typename cBuf>
	static void Render(Graphics& gfx, const RendererData<typename vertexType, typename cBuf>& data,
		D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
#ifdef _DEBUG
		DxgiInfoManager& infoManager = gfx.getInfoManager();
#endif
		if (data.vertexBufferSize == 0 || data.indexBufferSize == 0) return;

		UINT offset = 0;
		UINT stride = sizeof(vertexType);

		gfx.pContext->IASetVertexBuffers(0, 1, data.pVertexBuffer.GetAddressOf(), &stride, &offset);

		gfx.pContext->IASetIndexBuffer(data.pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		gfx.pContext->VSSetShader(data.pVertexShader.Get(), nullptr, 0);

		gfx.pContext->PSSetShader(data.pPixelShader.Get(), nullptr, 0);

		gfx.pContext->IASetInputLayout(data.pInputLayout.Get());

		gfx.pContext->IASetPrimitiveTopology(topology);

		if (data.hasTexture) {
			gfx.pContext->PSSetShaderResources(0, 1, data.pTextureView.GetAddressOf());
			gfx.pContext->PSSetSamplers(0, 1, data.pSampler.GetAddressOf());
		}

		gfx.pContext->VSSetConstantBuffers(0, 1, data.pConstantBuffer.GetAddressOf());

		gfx.pContext->DrawIndexed(data.indexBufferSize, 0, 0);
	}
};