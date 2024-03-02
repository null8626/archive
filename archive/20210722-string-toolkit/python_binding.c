#define PY_SSIZE_T_CLEAN
#include <Python.h>

#ifndef UNICODE
#define UNICODE
#endif

#pragma warning(disable: 4244)
#include "string_toolkit.h"
#pragma warning(default: 4244)

static PyObject * progress_bar(PyObject * self, PyObject * args) {
    double in_total, total;
    Py_ssize_t bar_length;
    wchar_t * elapsed_char;
    wchar_t * progress_bar;
    wchar_t * empty_char;
    
    if (!PyArg_ParseTuple(args, "dduuun", &in_total, &total, &elapsed_char, &progress_bar, &empty_char, &bar_length))
        return NULL;
    else if (wcslen(elapsed_char) != 1 || wcslen(progress_bar) != 1 || wcslen(empty_char) != 1) {
        PyErr_SetString(PyExc_TypeError, "The string length for the third, fourth, and fifth argument must be 1.");
        return NULL;
    } else if (bar_length < 1) {
        PyErr_SetString(PyExc_ValueError, "The bar length must be higher than 0.");
        return NULL;
    } else if (in_total > total) {
        PyErr_SetString(PyExc_TypeError, "The second argument must be higher than the first argument.");
        return NULL;
    } else if (in_total < 0 || total < 1) {
        PyErr_SetString(PyExc_ValueError, "Invalid in_total and total value.");
        return NULL;
    }
    
    wchar_t * output = st_progress_bar(in_total, total, elapsed_char[0], progress_bar[0], empty_char[0], (unsigned long long)bar_length);
    if (output == NULL)
        return Py_BuildValue("s", "", 0);
    return Py_BuildValue("u", output, wcslen(output));
}

static PyObject * get_indexes(PyObject * self, PyObject * args) {
    wchar_t * string;
    wchar_t * character;

    if (!PyArg_ParseTuple(args, "uu", &string, &character))
        return NULL;
    
    if (wcslen(character) != 1) {
        PyErr_SetString(PyExc_TypeError, "The string length for the second argument must be 1.");
        return NULL;
    }

    unsigned long long * array = st_get_indexes(string, character[0]);
    if (array == NULL)
        return PyList_New(0);
    
    PyObject * list = PyList_New(sizeof(array) / sizeof(array[0]));
    for (unsigned long long i = 0; i < sizeof(array) / sizeof(array[0]); i++)
        PyList_SET_ITEM(list, i, PyLong_FromUnsignedLongLong(array[i]));
    
    return list;
}

static PyObject * to_chunks(PyObject * self, PyObject * args) {
    wchar_t * string;
    Py_ssize_t amount;

    if (!PyArg_ParseTuple(args, "un", &string, &amount))
        return NULL;
    
    if (amount < 0) {
        PyErr_SetString(PyExc_TypeError, "The second argument must not be less than 0.");
        return NULL;
    }
    
    unsigned long long size;

    wchar_t ** array = st_to_chunks(string, (unsigned long long)amount, &size);
    if (array == NULL) {
        PyObject * list = PyList_New(1);
        PyList_SET_ITEM(list, 0, Py_BuildValue("u", string, wcslen(string)));
        return list;
    }

    PyObject * list = PyList_New(size);
    for (unsigned long long i = 0; i < size; i++)
        PyList_SET_ITEM(list, i, Py_BuildValue("u", array[i], wcslen(array[i])));
    
    return list;
}

static PyObject * substr(PyObject * self, PyObject * args) {
    wchar_t * string;
    Py_ssize_t a;
    Py_ssize_t b;

    if (!PyArg_ParseTuple(args, "unn", &string, &a, &b))
        return NULL;
    
    if (a < 0 || b < 0) {
        PyErr_SetString(PyExc_TypeError, "The second/third argument must not be less than 0.");
        return NULL;
    }

    wchar_t * output = st_substr(string, (unsigned long long)a, (unsigned long long)b);
    if (output == NULL)
        return Py_BuildValue("u", string, wcslen(string));
    
    return Py_BuildValue("u", output, wcslen(output));
}

static PyObject * shorten(PyObject * self, PyObject * args) {
    wchar_t * string;
    wchar_t * placeholder;
    Py_ssize_t amount;

    if (!PyArg_ParseTuple(args, "unu", &string, &amount, &placeholder))
        return NULL;
    
    if (amount < 1) {
        PyErr_SetString(PyExc_TypeError, "The amount must be higher than 1.");
        return NULL;
    }

    wchar_t * output = st_shorten(string, (unsigned long long)amount, placeholder);
    if (output == NULL)
        return Py_BuildValue("u", string, wcslen(string));
    
    return Py_BuildValue("u", output, wcslen(output));
}

static PyObject * has_custom_emoji(PyObject * self, PyObject * args) {
    wchar_t * string;
    if (!PyArg_ParseTuple(args, "u", &string))
        return NULL;
    
    return st_has_custom_emoji(string) ? Py_True : Py_False;
}

static PyObject * scramble(PyObject * self, PyObject * args) {
    wchar_t * string;
    if (!PyArg_ParseTuple(args, "u", &string))
        return NULL;
    
    st_scramble(string);
    return Py_BuildValue("u", string, wcslen(string));
}

static PyObject * proper_case(PyObject * self, PyObject * args) {
    wchar_t * string;
    if (!PyArg_ParseTuple(args, "u", &string))
        return NULL;
    
    st_proper_case(string);
    return Py_BuildValue("u", string, wcslen(string));
}

static PyObject * mock(PyObject * self, PyObject * args) {
    wchar_t * string;
    if (!PyArg_ParseTuple(args, "u", &string))
        return NULL;
    
    st_mock(string);
    return Py_BuildValue("u", string, wcslen(string));
}

static PyObject * to_abbreviation(PyObject * self, PyObject * args) {
    wchar_t * string;
    if (!PyArg_ParseTuple(args, "u", &string))
        return NULL;
    
    wchar_t * output = st_to_abbreviation(string);
    if (output == NULL) {
        PyErr_SetString(PyExc_TypeError, "The string length must be longer than 1 character.");
        return NULL;
    }
    
    return Py_BuildValue("u", output, wcslen(output));
}

static PyMethodDef string_toolkit_methods[] = {
    { "mock",             mock,             METH_VARARGS, "Mock a string" },
    { "to_abbreviation",  to_abbreviation,  METH_VARARGS, "Converts a string to it's abbreviation" },
    { "proper_case",      proper_case,      METH_VARARGS, "Fixes(?) the case of a string." },
    { "scramble",         scramble,         METH_VARARGS, "Scrambles a string." },
    { "has_custom_emoji", has_custom_emoji, METH_VARARGS, "Checks if a string contains a discord custom emoji or not." },
    { "shorten",          shorten,          METH_VARARGS, "Shortens a string." },
    { "substr",           substr,           METH_VARARGS, "Gets a sub-string from a string." },
    { "to_chunks",        to_chunks,        METH_VARARGS, "Converts a string to chunks." },
    { "get_indexes",      get_indexes,      METH_VARARGS, "Gets a list of indexes (matches) from a character." },
    { "progress_bar",     progress_bar,     METH_VARARGS, "Creates a progress bar from a string." },
    { NULL, NULL }
};

static struct PyModuleDef py_module = {
    PyModuleDef_HEAD_INIT,
    "string_toolkit",
    "Python wrapper for the string_toolkit.h C/C++ header",
    -1,
    string_toolkit_methods
};

PyMODINIT_FUNC PyInit_st(void) {
    return PyModule_Create(&py_module);
}