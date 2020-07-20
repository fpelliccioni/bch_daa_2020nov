#ifndef ASERTI3_416_PYAPI_H_
#define ASERTI3_416_PYAPI_H_

#include <Python.h>

#ifdef __cplusplus
extern "C" {  
#endif  

// class CBlockIndex --------------------------------------------------------
PyObject* PyAPI_CBlockIndex_construct(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockIndex_destruct(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockIndex_set_nHeight(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockIndex_get_nHeight(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockIndex_set_nTime(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockIndex_set_nBits(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockIndex_set_pprev(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockIndex_set_nChainWork(PyObject* self, PyObject* args);

// class CBlockHeader --------------------------------------------------------
PyObject* PyAPI_CBlockHeader_construct(PyObject* self, PyObject* args);
PyObject* PyAPI_CBlockHeader_destruct(PyObject* self, PyObject* args);

// class arith_uint256 --------------------------------------------------------
PyObject* PyAPI_arith_uint256_construct(PyObject* self, PyObject* args);
PyObject* PyAPI_arith_uint256_destruct(PyObject* self, PyObject* args);

// Parameters --------------------------------------------------------
PyObject* PyAPI_Params_GetDefaultMainnetConsensusParams(PyObject* self, PyObject* args);
PyObject* PyAPI_Params_destruct(PyObject* self, PyObject* args);

// GetNextASERTWorkRequired --------------------------------------------------------
PyObject* PyAPI_GetNextASERTWorkRequired(PyObject* self, PyObject* args);

#ifdef __cplusplus
} // extern "C"
#endif  


#endif // ASERTI3_416_PYAPI_H_