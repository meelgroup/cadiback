#pragma once
#include <cstdint>
#include <vector>

#if defined _WIN32
// dllexport only while building the DLL itself, see cadical_export.hpp
#ifdef cadiback_EXPORTS
#define CADIBACK_API __declspec (dllexport)
#else
#define CADIBACK_API
#endif
#else
#define CADIBACK_API __attribute__ ((visibility ("default")))
#endif

namespace CadiBack {
CADIBACK_API int doit (const std::vector<int>& cnf,
    int _verb,
    std::vector<int>& drop_cands,
    std::vector<int>& ret_backbone,
    std::vector<int>& ret_red_cls,
    std::vector<std::pair<int, int>>& ret_eqlits,
    int64_t max_confl = -1,
    bool* limit_hit = nullptr);

CADIBACK_API const char* get_version_sha1();
}
