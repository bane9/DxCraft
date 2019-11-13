#pragma once
#include "WinConfig.h"
#include <wrl.h>
#include <vector>
#include <string>
#include <dxgidebug.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	void set() noexcept;
	std::vector<std::string> getMessages() const;
private:
	unsigned long long next = 0;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};