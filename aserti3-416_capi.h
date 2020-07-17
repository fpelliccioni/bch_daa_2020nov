#ifndef ASERTI3_416_CAPI_H_
#define ASERTI3_416_CAPI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {  
#endif  

uint32_t CAPI_GetNextASERTWorkRequired(const void* pindexPrev,
                                  const void* pblock,
                                  const void* params,
                                  const int32_t nforkHeight);
}

#ifdef __cplusplus
} // extern "C"
#endif  


#endif // ASERTI3_416_CAPI_H_