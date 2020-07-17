#include <Python.h>
#include "aserti3-416_pyapi.h"

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

PyObject* PyAPI_GetNextASERTWorkRequired(PyObject* self, PyObject* args) {
    PyObject* py_pindexPrev;
    PyObject* py_pblock;
    PyObject* py_params;
    int32_t nforkHeight;

    if ( ! PyArg_ParseTuple(args, "OOOi", &py_pindexPrev, &py_pblock, &py_params, &nforkHeight)) {
        return NULL;
    }

    void* pindexPrev = get_ptr(py_pindexPrev);
    void* pblock = get_ptr(py_pblock);
    void* params = get_ptr(py_params);

    uint32_t res = CAPI_GetNextASERTWorkRequired(pindexPrev, pblock, params, nforkHeight);
    return Py_BuildValue("I", res);   
}

#ifdef __cplusplus
} // extern "C"
#endif  
