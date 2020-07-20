#ifndef ASERTI3_416_CAPI_H_
#define ASERTI3_416_CAPI_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// class CBlockIndex --------------------------------------------------------
void* CAPI_CBlockIndex_construct(void);
void CAPI_CBlockIndex_destruct(void* ptr);
void CAPI_CBlockIndex_set_nHeight(void* ptr, int nHeight);
int CAPI_CBlockIndex_get_nHeight(void* ptr);
void CAPI_CBlockIndex_set_nTime(void* ptr, uint32_t nTime);
void CAPI_CBlockIndex_set_nBits(void* ptr, uint32_t nBits);
void CAPI_CBlockIndex_set_pprev(void* ptr, void* pprev_ptr);
void CAPI_CBlockIndex_set_nChainWork(void* ptr, void* nChainWork);

// class CBlockHeader --------------------------------------------------------
void* CAPI_CBlockHeader_construct(void);
void CAPI_CBlockHeader_destruct(void* ptr);

// class arith_uint256 --------------------------------------------------------
void* CAPI_arith_uint256_construct(void);
void CAPI_arith_uint256_destruct(void* ptr);
// arith_uint256 &SetCompact(uint32_t nCompact, bool *pfNegative = nullptr, bool *pfOverflow = nullptr);
void CAPI_arith_uint256_SetCompact(void* ptr, uint32_t nCompact, int* pfNegative_par /*= NULL*/, int* pfOverflow_par /*= NULL*/);
void* CAPI_arith_uint256_complement(void* ptr);
int CAPI_arith_uint256_equal_to(void* ptr, uint64_t b);
void* CAPI_arith_uint256_quotient(void* ptr, uint64_t b);
void* CAPI_arith_uint256_add(void* ptr, uint64_t b);


// Parameters --------------------------------------------------------
void* CAPI_Params_GetDefaultMainnetConsensusParams(void);
void CAPI_Params_destruct(void* ptr);


// CAPI_GetNextASERTWorkRequired --------------------------------------------------------
uint32_t CAPI_GetNextASERTWorkRequired(const void* pindexPrev,
                                  const void* pblock,
                                  const void* params,
                                  const int32_t nforkHeight);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ASERTI3_416_CAPI_H_