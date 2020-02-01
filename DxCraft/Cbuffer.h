#pragma once
#include <type_traits>
#include <d3d11.h>
#include <wrl.h>
#include <typeinfo>
#include "Graphics.h"

struct Cbuffer {
	Cbuffer() {
		type = const_cast<type_info*>(&typeid(this));
	}

	template<typename T>
	void Update(const T& data, Graphics& gfx) {
		if (*type != typeid(T) || buffer == nullptr) Upload(data, gfx);
		D3D11_MAPPED_SUBRESOURCE msr;
		gfx.pContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, &data, sizeof(data));
		gfx.pContext->Unmap(buffer.Get(), 0);
	}

	template<typename T>
	void Upload(const T& data, Graphics& gfx) {
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0;
		cbd.ByteWidth = sizeof(data);
		cbd.StructureByteStride = 0;
		INFOMAN(gfx);
		GFX_EXCEPT_INFO(gfx.pDevice->CreateBuffer(&cbd, nullptr, buffer.ReleaseAndGetAddressOf()));

		type = const_cast<type_info*>(&typeid(T));
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
private:
	type_info* type;
};
