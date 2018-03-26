#ifndef GAMMOU_PLUGIN_HELPER_H_
#define GAMMOU_PLUGIN_HELPER_H_

#ifdef _WIN32
#define DYNAMIC_EXPORT(decl) extern "C" { __declspec(dllexport) decl }
#else
#define DYNAMIC_EXPORT(decl) extern "C" { decl }
#endif


#endif
