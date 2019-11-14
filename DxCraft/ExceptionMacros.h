#pragma once
#include "CustomException.h"

#define FAILED_EXCEPT(type, hr) if(FAILED(hr)) throw CustomException(__LINE__, __FILE__, type, hr)
#define FAILED_LAST_EXCEPT(type, b) if(!b) throw CustomException(__LINE__, __FILE__, type, GetLastError())
#define FAILED_EXCEPT_NOINFO(type, hr) if(FAILED(hr)) throw CustomException(__LINE__, __FILE__, type, hr)
#define FAILED_LAST_EXCEPT_NOINFO(type, b) if(!b) throw CustomException(__LINE__, __FILE__, type, GetLastError())
#define THROW_EXCEPT(type, hr) throw CustomException(__LINE__, __FILE__, type, hr)

#ifdef _DEBUG
#define GFX_EXCEPT(hr) if(FAILED(hr)) throw CustomException(__LINE__, __FILE__, "Graphics Exception", hr, infoManager.getMessages())
#define GFX_EXCEPT_LAST(b) if(!b) throw CustomException(__LINE__, __FILE__, "Graphics Exception", GetLastError(), infoManager.getMessages())
#define GFX_EXCEPT_THROW(hr) throw CustomException(__LINE__, __FILE__, "Graphics Exception", hr, infoManager.getMessages())
#define GFX_EXCEPT_INFO(hr) infoManager.set();  GFX_EXCEPT(hr)
#define GFX_EXCEPT_LAST_INFO(b) infoManager.set();  GFX_EXCEPT_LAST(b)
#else
#define GFX_EXCEPT(hr) if(FAILED(hr)) throw CustomException(__LINE__, __FILE__, "Graphics Exception", hr)
#define GFX_EXCEPT_LAST(b) if(!b) throw CustomException(__LINE__, __FILE__, "Graphics Exception", GetLastError())
#define GFX_EXCEPT_THROW(hr) throw CustomException(__LINE__, __FILE__, "Graphics Exception", hr)
#define GFX_EXCEPT_INFO(hr) GFX_EXCEPT(hr)
#define GFX_EXCEPT_LAST_INFO(b) GFX_EXCEPT_LAST(b)
#endif

#ifdef _DEBUG
#define INFOMAN(gfx) DxgiInfoManager infoManager& = gfx.getInfoManager()
#else
#define INFOMAN(gfx)
#endif