#include "Python.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	int clusterSize = 12;
	int clusterCount = 12;
	PyObject *pName, *pModule, *pDict, *pArgs,
	         *pClass, *pInstance, *pValue, *pReturn;
	
	
	
	
	Py_Initialize();
	// set python sys path

	
	PySys_SetArgv(argc, argv);
	// Build the name object
	pName = PyString_FromString("Iterative1AAM");

	// Load the module object
	// pModule = PyImport_Import(pName);
	pModule = PyImport_ImportModule("Iterative1AAM");

	// pDict is a borrowed reference 
	pDict = PyModule_GetDict(pModule);
	// DEBUG ------------------------
	PyObject *key, *value;
	Py_ssize_t pos = 0;

	while (PyDict_Next(pDict, &pos, &key, &value)) {
		printf("Key %d is %s\n", pos, PyString_AsString(key));
	}
	// ---------------------------
	// Build the name of a callable class 
	pClass = PyDict_GetItemString(pDict, "Iterative1AAM");


    // Create an instance of the class
	pArgs = PyTuple_Pack(2, PyInt_FromLong(clusterSize), PyInt_FromLong(clusterCount));
	pInstance = PyObject_CallObject(pClass, pArgs);
	printf("%d != 0 should be true\n", pInstance);

	// test store
	PyObject *storeMessage;
	storeMessage = PyList_New(clusterCount);
	for (int i = 0; i< clusterCount; i++)
		PyList_SetItem(storeMessage, i, PyInt_FromLong(i));
	
	PyObject_CallMethodObjArgs(pInstance, PyString_FromString("storeMessage"), storeMessage, 0);
	Py_DECREF(storeMessage);
	if (PyErr_Occurred())
		PyErr_Print();
	// test read
	PyObject *readMessage;
	readMessage = PyList_New(clusterCount);
	for (int i = 0; i< clusterCount; i++)
		PyList_SetItem(readMessage, i, PyInt_FromLong(i));
	
	
	
	printf("does %d == 8 ?\n", PyInt_AsLong(PySequence_GetItem(readMessage, 8)));
	Py_INCREF(Py_None);
	PyList_SetItem(readMessage, 8, Py_None);

	
	pReturn = PyObject_CallMethodObjArgs(pInstance, PyString_FromString("readMessage"), readMessage, PyInt_FromLong(1), 0);
	
	printf("does %d == 8 ?\n", PyInt_AsLong(PySequence_GetItem(readMessage, 8)));
	printf("iterations: %d\n", PyInt_AsLong(pReturn));
	
	if (PyErr_Occurred())
		PyErr_Print();
	// Clean up
	Py_DECREF(pModule);
	Py_DECREF(pName);
	
	Py_Finalize();
	return 0;
}
