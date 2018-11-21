#pragma once
#ifdef INJECTDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif // MYDLL_EXPORTS

extern "C" MYDLL_API int funmydll(void);