#include <Python.h>
#include "aserti3-416_pyapi.h"
#include "aserti3-416_capi.h"

#ifdef __cplusplus
extern "C" {  
#endif  

void* get_ptr(PyObject* obj) {
#if PY_MAJOR_VERSION >= 3
    return PyCapsule_GetPointer(obj, NULL);
#else /* PY_MAJOR_VERSION >= 3 */
    return PyCObject_AsVoidPtr(obj);
#endif /* PY_MAJOR_VERSION >= 3 */
}

// inline
PyObject* to_py_obj(void* obj) {
#if PY_MAJOR_VERSION >= 3
    return PyCapsule_New(obj, NULL, NULL);
#else /* PY_MAJOR_VERSION >= 3 */
    return PyCObject_FromVoidPtr(obj, NULL);
#endif /* PY_MAJOR_VERSION >= 3 */
}




// class CBlockIndex --------------------------------------------------------
PyObject* PyAPI_CBlockIndex_construct(PyObject* self, PyObject* args) {
    void* res = CAPI_CBlockIndex_construct();
    return to_py_obj(res);
}

PyObject* PyAPI_CBlockIndex_destruct(PyObject* self, PyObject* args) {
    PyObject* py_obj;

    if ( ! PyArg_ParseTuple(args, "O", &py_obj)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    CAPI_CBlockIndex_destruct(obj);

    Py_RETURN_NONE;
}

PyObject* PyAPI_CBlockIndex_set_nHeight(PyObject* self, PyObject* args) {
    PyObject* py_obj;
    int nHeight;

    if ( ! PyArg_ParseTuple(args, "Oi", &py_obj, &nHeight)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    CAPI_CBlockIndex_set_nHeight(obj, nHeight);

    Py_RETURN_NONE;
}

// int CAPI_CBlockIndex_get_nHeight(void* ptr);
PyObject* PyAPI_CBlockIndex_get_nHeight(PyObject* self, PyObject* args) {
    PyObject* py_obj;

    if ( ! PyArg_ParseTuple(args, "O", &py_obj)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    int res = CAPI_CBlockIndex_get_nHeight(obj);

    return Py_BuildValue("i", res);
}



PyObject* PyAPI_CBlockIndex_set_nTime(PyObject* self, PyObject* args) {
    PyObject* py_obj;
    uint32_t nTime;

    if ( ! PyArg_ParseTuple(args, "Oi", &py_obj, &nTime)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    CAPI_CBlockIndex_set_nTime(obj, nTime);

    Py_RETURN_NONE;
}

PyObject* PyAPI_CBlockIndex_set_nBits(PyObject* self, PyObject* args) {
    PyObject* py_obj;
    uint32_t nBits;

    if ( ! PyArg_ParseTuple(args, "Oi", &py_obj, &nBits)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    CAPI_CBlockIndex_set_nBits(obj, nBits);

    Py_RETURN_NONE;
}

PyObject* PyAPI_CBlockIndex_set_pprev(PyObject* self, PyObject* args) {
    PyObject* py_obj;
    PyObject* py_pprev;

    if ( ! PyArg_ParseTuple(args, "OO", &py_obj, &py_pprev)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    void* pprev = get_ptr(py_pprev);
    CAPI_CBlockIndex_set_pprev(obj, pprev);

    Py_RETURN_NONE;
}



PyObject* PyAPI_CBlockIndex_set_nChainWork(PyObject* self, PyObject* args) {
    PyObject* py_obj;
    PyObject* py_nChainWork;

    if ( ! PyArg_ParseTuple(args, "OO", &py_obj, &py_nChainWork)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    void* nChainWork = get_ptr(py_nChainWork);
    CAPI_CBlockIndex_set_nChainWork(obj, nChainWork);

    Py_RETURN_NONE;
}

// class CBlockHeader --------------------------------------------------------
PyObject* PyAPI_CBlockHeader_construct(PyObject* self, PyObject* args) {
    void* res = CAPI_CBlockHeader_construct();
    return to_py_obj(res);
}

PyObject* PyAPI_CBlockHeader_destruct(PyObject* self, PyObject* args) {
    PyObject* py_obj;

    if ( ! PyArg_ParseTuple(args, "O", &py_obj)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    CAPI_CBlockHeader_destruct(obj);

    Py_RETURN_NONE;
}


// class arith_uint256 --------------------------------------------------------
PyObject* PyAPI_arith_uint256_construct(PyObject* self, PyObject* args) {
    void* res = CAPI_arith_uint256_construct();
    return to_py_obj(res);
}

PyObject* PyAPI_arith_uint256_destruct(PyObject* self, PyObject* args) {
    PyObject* py_obj;

    if ( ! PyArg_ParseTuple(args, "O", &py_obj)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    CAPI_arith_uint256_destruct(obj);

    Py_RETURN_NONE;
}

// // arith_uint256 &SetCompact(uint32_t nCompact, bool *pfNegative = nullptr, bool *pfOverflow = nullptr);
// void CAPI_arith_uint256_SetCompact(void* ptr, uint32_t nCompact, int* pfNegative_par = NULL, int* pfOverflow_par = NULL);
// void* CAPI_arith_uint256_complement(void* ptr);
// int CAPI_arith_uint256_equal_to(void* ptr, uint64_t b);
// void* CAPI_arith_uint256_quotient(void* ptr, uint64_t b);
// void* CAPI_arith_uint256_add(void* ptr, uint64_t b);



// Parameters --------------------------------------------------------
PyObject* PyAPI_Params_GetDefaultMainnetConsensusParams(PyObject* self, PyObject* args) {
    void* res = CAPI_Params_GetDefaultMainnetConsensusParams();
    return to_py_obj(res);
}

PyObject* PyAPI_Params_destruct(PyObject* self, PyObject* args) {
    PyObject* py_obj;

    if ( ! PyArg_ParseTuple(args, "O", &py_obj)) {
        return NULL;
    }
    void* obj = get_ptr(py_obj);
    CAPI_Params_destruct(obj);

    Py_RETURN_NONE;
}


// GetNextASERTWorkRequired --------------------------------------------------------
PyObject* PyAPI_GetNextASERTWorkRequired(PyObject* self, PyObject* args) {
    PyObject* py_pindexPrev;
    PyObject* py_pblock;
    PyObject* py_params;
    PyObject* py_pindexReferenceBlock;
    int debugASERT;

    if ( ! PyArg_ParseTuple(args, "OOOOp", &py_pindexPrev, &py_pblock, &py_params, &py_pindexReferenceBlock, &debugASERT)) {
        return NULL;
    }

    void* pindexPrev = get_ptr(py_pindexPrev);
    void* pblock = get_ptr(py_pblock);
    void* params = get_ptr(py_params);
    void* pindexReferenceBlock = get_ptr(py_pindexReferenceBlock);

    uint32_t res = CAPI_GetNextASERTWorkRequired(pindexPrev, pblock, params, pindexReferenceBlock, debugASERT);
    return Py_BuildValue("I", res);   
}


// PyObject* PyAPI_GetNextASERTWorkRequired(PyObject* self, PyObject* args) {
//     PyObject* py_pindexPrev;
//     PyObject* py_pblock;
//     PyObject* py_params;
//     int32_t nforkHeight;

//     if ( ! PyArg_ParseTuple(args, "OOOi", &py_pindexPrev, &py_pblock, &py_params, &nforkHeight)) {
//         return NULL;
//     }

//     void* pindexPrev = get_ptr(py_pindexPrev);
//     void* pblock = get_ptr(py_pblock);
//     void* params = get_ptr(py_params);

//     uint32_t res = CAPI_GetNextASERTWorkRequired(pindexPrev, pblock, params, nforkHeight);
//     return Py_BuildValue("I", res);   
// }

#ifdef __cplusplus
} // extern "C"
#endif  
