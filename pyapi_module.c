/**
 * Copyright (c) 2020 Fernando Pelliccioni
 */


#include <Python.h>
#include "aserti3-416_pyapi.h"


#ifdef __cplusplus
extern "C" {  
#endif  


static
PyMethodDef NativeMethods[] = {

    // class CBlockIndex --------------------------------------------------------
    {"CBlockIndex_construct",      PyAPI_CBlockIndex_construct, METH_VARARGS, ""},
    {"CBlockIndex_destruct",       PyAPI_CBlockIndex_destruct, METH_VARARGS, ""},
    {"CBlockIndex_set_nHeight",    PyAPI_CBlockIndex_set_nHeight, METH_VARARGS, ""},
    {"CBlockIndex_get_nHeight",    PyAPI_CBlockIndex_get_nHeight, METH_VARARGS, ""},
    {"CBlockIndex_set_nTime",      PyAPI_CBlockIndex_set_nTime, METH_VARARGS, ""},
    {"CBlockIndex_set_nBits",      PyAPI_CBlockIndex_set_nBits, METH_VARARGS, ""},
    {"CBlockIndex_set_pprev",      PyAPI_CBlockIndex_set_pprev, METH_VARARGS, ""},
    {"CBlockIndex_set_nChainWork", PyAPI_CBlockIndex_set_nChainWork, METH_VARARGS, ""},

    // class CBlockHeader --------------------------------------------------------
    {"CBlockHeader_construct", PyAPI_CBlockHeader_construct, METH_VARARGS, ""},
    {"CBlockHeader_destruct",  PyAPI_CBlockHeader_destruct, METH_VARARGS, ""},

    // class arith_uint256 --------------------------------------------------------
    {"arith_uint256_construct", PyAPI_arith_uint256_construct, METH_VARARGS, ""},
    {"arith_uint256_destruct",  PyAPI_arith_uint256_destruct, METH_VARARGS, ""},

    // Parameters --------------------------------------------------------
    {"Params_GetDefaultMainnetConsensusParams",  PyAPI_Params_GetDefaultMainnetConsensusParams, METH_VARARGS, ""},
    {"Params_destruct",  PyAPI_Params_destruct, METH_VARARGS, ""},

    // GetNextASERTWorkRequired --------------------------------------------------------
    {"GetNextASERTWorkRequired",  PyAPI_GetNextASERTWorkRequired, METH_VARARGS, ""},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else //PY_MAJOR_VERSION >= 3
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif //PY_MAJOR_VERSION >= 3

#if PY_MAJOR_VERSION >= 3

static 
int myextension_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static 
int myextension_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static 
struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "aserti3416cpp",
        NULL,
        sizeof(struct module_state),
        NativeMethods,
        NULL,
        myextension_traverse,
        myextension_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_aserti3416cpp(void)


#else /* PY_MAJOR_VERSION >= 3 */

#define INITERROR return

void /*PyMODINIT_FUNC*/
initaserti3416cpp(void)

#endif /* PY_MAJOR_VERSION >= 3 */


{

    // Make sure the GIL has been created since we need to acquire it in our
    // callback to safely call into the python application.
    if (! PyEval_ThreadsInitialized()) {
        PyEval_InitThreads();
    }


#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("aserti3416cpp", NativeMethods);
    // (void) Py_InitModule("aserti3416cpp", NativeMethods);
#endif

    if (module == NULL) {
        INITERROR;
    }

    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException((char*)"myextension.Error", NULL, NULL);
    
    if (st->error == NULL) {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}

#ifdef __cplusplus
} // extern "C"
#endif
