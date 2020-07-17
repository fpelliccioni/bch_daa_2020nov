// g++ -std=c++11 aserti3-416.cpp

#include <stdint.h>

#include "aserti3-416_capi.h"
#include "aserti3-416.hpp"

extern "C" {  

uint32_t CAPI_GetNextASERTWorkRequired(void const* pindexPrev,
                                  void const* pblock,
                                  void const* params,
                                  const int32_t nforkHeight) {

    CBlockIndex const* pindexPrev_cpp = static_cast<CBlockIndex const*>(pindexPrev);
    CBlockHeader const* pblock_cpp = static_cast<CBlockHeader const*>(pblock);
    Consensus::Params const& params_cpp = *static_cast<Consensus::Params const*>(params);

    return GetNextASERTWorkRequired(pindexPrev_cpp, pblock_cpp, params_cpp, nforkHeight);
}

#ifdef __cplusplus
} // extern "C"
#endif  
