#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include "ExceptionMacros.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")


DxgiInfoManager::DxgiInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const auto hModDxgiDebug = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	FAILED_LAST_EXCEPT_NOINFO("DxgiInfoManager setup error", hModDxgiDebug);

	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	FAILED_LAST_EXCEPT_NOINFO("DxgiInfoManager setup error", DxgiGetDebugInterface);

	FAILED_EXCEPT_NOINFO("DxgiInfoManager setup error", DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
}

void DxgiInfoManager::set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::getMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		SIZE_T messageLength{};
		FAILED_EXCEPT_NOINFO("DxgiInfoManager error", pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		FAILED_EXCEPT_NOINFO("DxgiInfoManager error", pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}