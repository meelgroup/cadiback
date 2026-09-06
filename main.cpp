// Runs CadiBack::doit() on a DIMACS file, standalone.
// Usage: cadiback-cli [--verb N] [--maxconfl N] [--drop v1,v2,...] file.cnf

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>

#include "cadiback.h"

static void die(const char* msg, const char* arg = nullptr) {
    fprintf(stderr, "ERROR: %s%s%s\n", msg, arg ? " " : "", arg ? arg : "");
    exit(1);
}

static std::vector<int> read_dimacs(const char* fname, int& vars) {
    FILE* f = fopen(fname, "r");
    if (!f) die("cannot open", fname);

    std::vector<int> cnf;
    vars = 0;
    int cls_in_header = 0;
    char line[1 << 16];
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == 'c' || *p == '\n' || *p == '\0') continue;
        if (*p == 'p') {
            if (sscanf(p, "p cnf %d %d", &vars, &cls_in_header) != 2)
                die("bad 'p cnf' header in", fname);
            continue;
        }
        // A clause may be spread over several lines, so just consume ints.
        while (*p) {
            while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
            if (!*p) break;
            char* end;
            long v = strtol(p, &end, 10);
            if (end == p) die("bad literal in", fname);
            cnf.push_back((int)v);
            p = end;
        }
    }
    fclose(f);

    if (!cnf.empty() && cnf.back() != 0) die("last clause not 0-terminated in", fname);
    return cnf;
}

int main(int argc, char** argv) {
    int verb = 1;
    int64_t max_confl = -1;
    std::vector<int> drop_cands;
    const char* fname = nullptr;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--verb") && i + 1 < argc) verb = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--maxconfl") && i + 1 < argc) max_confl = atoll(argv[++i]);
        else if (!strcmp(argv[i], "--drop") && i + 1 < argc) {
            for (const char* p = argv[++i]; *p; ) {
                char* end;
                long v = strtol(p, &end, 10);
                if (end == p) break;
                drop_cands.push_back((int)v);
                p = (*end == ',') ? end + 1 : end;
            }
        } else if (argv[i][0] == '-') die("unknown option", argv[i]);
        else fname = argv[i];
    }
    if (!fname) die("no input file given");

    int vars = 0;
    std::vector<int> cnf = read_dimacs(fname, vars);

    size_t num_cls = 0, num_lits = 0;
    for (const int l : cnf) { if (l == 0) num_cls++; else num_lits++; }
    printf("c o [cadiback-cli] %s -- vars: %d cls: %zu lits: %zu drop_cands: %zu maxconfl: %ld\n",
           fname, vars, num_cls, num_lits, drop_cands.size(), (long)max_confl);
    fflush(stdout);

    std::vector<int> ret_backbone, ret_red_cls;
    std::vector<std::pair<int, int>> ret_eqlits;
    bool limit_hit = false;
    int res = CadiBack::doit(cnf, verb, drop_cands, ret_backbone, ret_red_cls,
                             ret_eqlits, max_confl, &limit_hit);

    size_t num_bins = 0;
    for (const int l : ret_red_cls) if (l == 0) num_bins++;
    size_t num_units = 0;
    for (const int l : ret_backbone) if (l != 0) num_units++;
    printf("c o [cadiback-cli] res: %d units: %zu bins: %zu eqlits: %zu limit_hit: %d\n",
           res, num_units, num_bins, ret_eqlits.size(), (int)limit_hit);
    return 0;
}
