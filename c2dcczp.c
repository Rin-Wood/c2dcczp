#include <Python.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define ENCLEN 1024
#define SECURELEN (512 * 4)
#define DISTANCE (64 * 4)
#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (KEY[(p & 3) ^ e] ^ z)))
typedef uint8_t  byte;
typedef uint32_t uint;
static uint KEY[4];
static uint DECKEY[ENCLEN];

static void InitializeKey(uint a, uint b, uint c, uint d) {
    uint y, p, e;
    uint rounds = 6;
    uint sum = 0;
    uint z = DECKEY[ENCLEN - 1];
    KEY[0] = a;
    KEY[1] = b;
    KEY[2] = c;
    KEY[3] = d;
    uint DELTA = 0x9e3779b9;

    do {
        sum += DELTA;
        e = (sum >> 2) & 3;

        for (p = 0; p < ENCLEN - 1; p++) {
            y = DECKEY[p + 1];
            z = DECKEY[p] += MX;
        }

        y = DECKEY[0];
        z = DECKEY[ENCLEN - 1] += MX;

    } while (--rounds > 0);
}

static void Decrypt(byte* bytes, int length) {
    uint b = 0;
    uint i = 0;
    uint len = length - 12;

    for (; i < len && i < SECURELEN; i += 4) {
        byte* key = (byte*)&DECKEY[b++];
        bytes[i + 0] ^= key[0];
        bytes[i + 1] ^= key[1];
        bytes[i + 2] ^= key[2];
        bytes[i + 3] ^= key[3];

        if (b >= ENCLEN) {
            b = 0;
        }
    }

    for (; i < len; i += DISTANCE) {
        byte* key = (byte*)&DECKEY[b++];
        bytes[i + 0] ^= key[0];
        bytes[i + 1] ^= key[1];
        bytes[i + 2] ^= key[2];
        bytes[i + 3] ^= key[3];

        if (b >= ENCLEN) {
            b = 0;
        }
    }
}

static PyObject* set_key(PyObject* self, PyObject* args) {
    uint key1, key2, key3, key4;
    if (!PyArg_ParseTuple(args, "IIII", &key1, &key2, &key3, &key4)) {
        return NULL;
    }
    InitializeKey(key1, key2, key3, key4);
    Py_RETURN_NONE;
}

static PyObject* decrypt(PyObject* self, PyObject* args) {
    Py_buffer data;
    if (!PyArg_ParseTuple(args, "y*", &data)) {
        return NULL;
    }

    int length = (int)data.len;
    byte* decrypted_data = (byte*)malloc(length);
    if (!decrypted_data) {
        PyBuffer_Release(&data);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for decrypted data.");
        return NULL;
    }

    memcpy(decrypted_data, data.buf, length);
    Decrypt(decrypted_data, length);
    PyObject* result = PyBytes_FromStringAndSize((const char*)decrypted_data, length);

    free(decrypted_data);
    PyBuffer_Release(&data);

    return result;
}

static PyMethodDef C2DCCZPMethods[] = {
    {"SetKey", set_key, METH_VARARGS, "Set encryption key"},
    {"decrypt", (PyCFunction)decrypt, METH_VARARGS, "Decrypt C2dCCZp"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef c2dcczpmodule = {
    PyModuleDef_HEAD_INIT,
    "c2dcczp",
    NULL,
    -1,
    C2DCCZPMethods
};

PyMODINIT_FUNC PyInit_c2dcczp(void) {
    return PyModule_Create(&c2dcczpmodule);
}
