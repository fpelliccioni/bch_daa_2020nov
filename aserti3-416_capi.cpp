// g++ -std=c++11 aserti3-416.cpp

#include <stdint.h>

#include "aserti3-416_capi.h"
#include "aserti3-416.hpp"

extern "C" {  

/*
CBlockIndex
CBlockIndex_GetBlockTime
pindexPrev->GetAncestor(nRefHeight);
pindexPrev->nHeight
pindexPrev->nTime
params.nPowTargetSpacing
*/

/*
// Genesis block.
blocks[0] = CBlockIndex();
blocks[0].nHeight = 0;
blocks[0].nTime = 1269211443;
blocks[0].nBits = initialBits;
blocks[0].nChainWork = GetBlockProof(blocks[0]);
*/

// class CBlockIndex --------------------------------------------------------
void* CAPI_CBlockIndex_construct() {
    return new CBlockIndex();
}

void CAPI_CBlockIndex_destruct(void* ptr) {
    auto* obj = static_cast<CBlockIndex*>(ptr);
    delete obj;
}

void CAPI_CBlockIndex_set_nHeight(void* ptr, int nHeight) {
    static_cast<CBlockIndex*>(ptr)->nHeight = nHeight;
}

int CAPI_CBlockIndex_get_nHeight(void* ptr) {
    return static_cast<CBlockIndex*>(ptr)->nHeight;
}

void CAPI_CBlockIndex_set_nTime(void* ptr, uint32_t nTime) {
    static_cast<CBlockIndex*>(ptr)->nTime = nTime;
}

void CAPI_CBlockIndex_set_nBits(void* ptr, uint32_t nBits) {
    static_cast<CBlockIndex*>(ptr)->nBits = nBits;
}

void CAPI_CBlockIndex_set_pprev(void* ptr, void* pprev_ptr) {
    static_cast<CBlockIndex*>(ptr)->pprev = static_cast<CBlockIndex*>(pprev_ptr);
}

//                                              arith_uint256
void CAPI_CBlockIndex_set_nChainWork(void* ptr, void* nChainWork) {
    static_cast<CBlockIndex*>(ptr)->nChainWork = *static_cast<arith_uint256*>(nChainWork);
}

// class CBlockHeader --------------------------------------------------------
void* CAPI_CBlockHeader_construct() {
    return new CBlockHeader();
}

void CAPI_CBlockHeader_destruct(void* ptr) {
    auto* obj = static_cast<CBlockHeader*>(ptr);
    delete obj;
}


// class arith_uint256 --------------------------------------------------------
void* CAPI_arith_uint256_construct() {
    return new arith_uint256();
}

void CAPI_arith_uint256_destruct(void* ptr) {
    auto* obj = static_cast<arith_uint256*>(ptr);
    delete obj;
}

// arith_uint256 &SetCompact(uint32_t nCompact, bool *pfNegative = nullptr, bool *pfOverflow = nullptr);
void CAPI_arith_uint256_SetCompact(void* ptr, uint32_t nCompact, int* pfNegative_par /*= NULL*/, int* pfOverflow_par /*= NULL*/) {
    bool pfNegative;
    bool pfOverflow;

    static_cast<arith_uint256*>(ptr)->SetCompact(nCompact, 
        pfNegative_par == NULL ? nullptr : &pfNegative,
        pfOverflow_par == NULL ? nullptr : &pfOverflow
    );

    if (pfNegative_par != NULL) {
        *pfNegative_par = pfNegative;
    }

    if (pfOverflow_par != NULL) {
        *pfOverflow_par = pfOverflow;
    }
}

void* CAPI_arith_uint256_complement(void* ptr) {
    auto* obj = static_cast<arith_uint256*>(ptr);
    auto newone = ~(*obj);
    return new arith_uint256(newone);
}

int CAPI_arith_uint256_equal_to(void* ptr, uint64_t b) {
    auto* obj = static_cast<arith_uint256*>(ptr);
    return *obj == b;
}

void* CAPI_arith_uint256_quotient(void* ptr, uint64_t b) {
    auto* obj = static_cast<arith_uint256*>(ptr);
    auto newone = *obj / b;
    return new arith_uint256(newone);
}

void* CAPI_arith_uint256_add(void* ptr, uint64_t b) {
    auto* obj = static_cast<arith_uint256*>(ptr);
    auto newone = *obj + b;
    return new arith_uint256(newone);
}


// Parameters --------------------------------------------------------

// inline
// Consensus::Params GetDefaultMainnetConsensusParams() {
//     Consensus::Params consensus;
//     consensus.powLimit = uint256S(
//         "00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
//     // two weeks
//     consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
//     consensus.nPowTargetSpacing = 10 * 60;
//     consensus.fPowAllowMinDifficultyBlocks = false;
//     return consensus; 
// }

void* CAPI_Params_GetDefaultMainnetConsensusParams() {
    Consensus::Params* consensus = new Consensus::Params;
    consensus->powLimit = uint256S(
        "00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    // two weeks
    consensus->nPowTargetTimespan = 14 * 24 * 60 * 60;
    consensus->nPowTargetSpacing = 10 * 60;
    consensus->fPowAllowMinDifficultyBlocks = false;
    consensus->nDAAHalfLife = 2 * 24 * 60 * 60;
    return consensus; 
}

void CAPI_Params_destruct(void* ptr) {
    auto* obj = static_cast<Consensus::Params*>(ptr);
    delete obj;
}


// CAPI_GetNextASERTWorkRequired --------------------------------------------------------
uint32_t CAPI_GetNextASERTWorkRequired(void const* pindexPrev,
                                  void const* pblock,
                                  void const* params,
                                  void const* pindexReferenceBlock,
                                  int debugASERT) {

    CBlockIndex const* pindexPrev_cpp = static_cast<CBlockIndex const*>(pindexPrev);
    CBlockHeader const* pblock_cpp = static_cast<CBlockHeader const*>(pblock);
    Consensus::Params const& params_cpp = *static_cast<Consensus::Params const*>(params);
    CBlockIndex const* pindexReferenceBlock_cpp = static_cast<CBlockIndex const*>(pindexReferenceBlock);

    return GetNextASERTWorkRequired(pindexPrev_cpp, pblock_cpp, params_cpp, pindexReferenceBlock_cpp, debugASERT);
}


// uint32_t CAPI_GetNextASERTWorkRequired(void const* pindexPrev,
//                                   void const* pblock,
//                                   void const* params,
//                                   const int32_t nforkHeight) {

//     CBlockIndex const* pindexPrev_cpp = static_cast<CBlockIndex const*>(pindexPrev);
//     CBlockHeader const* pblock_cpp = static_cast<CBlockHeader const*>(pblock);
//     Consensus::Params const& params_cpp = *static_cast<Consensus::Params const*>(params);

//     return GetNextASERTWorkRequired(pindexPrev_cpp, pblock_cpp, params_cpp, nforkHeight);
// }

} // extern "C"
