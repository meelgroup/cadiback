#pragma once
#include <vector>

namespace CadiBack {
int doit (const std::vector<int>& cnf,
    int _verb,
    std::vector<int>& drop_cands,
    std::vector<int>& ret_backbone);
}
