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
	PyObject *sysPath = PySys_GetObject("path");
	PyObject *path = PyString_FromString("../models/");
	PyList_Append(sysPath, path);
	
	
	// printf("%s\n", PyString_AsString(PySys_GetObject("path")));
	// build module/class name
	pName = PyString_FromString("Iterative1AAM");

	// Load the module object
	pModule = PyImport_Import(pName);
	
	// pDict is a borrowed reference 
	pDict = PyModule_GetDict(pModule);


	// Build the name of a callable class 
	pClass = PyDict_GetItem(pDict, pName);


    // Create an instance of the class
	pArgs = PyTuple_Pack(2, PyInt_FromLong(clusterSize), PyInt_FromLong(clusterCount));
	pInstance = PyObject_CallObject(pClass, pArgs);
	
	// Clean up
	Py_DECREF(pName);
	Py_DECREF(pModule);
	Py_DECREF(pDict);
	Py_DECREF(pClass);	
	Py_DECREF(pArgs);

	// test store
	PyObject *storeMessage;
	storeMessage = PyList_New(clusterCount);
	for (int i = 0; i< clusterCount; i++)
		PyList_SetItem(storeMessage, i, PyInt_FromLong(i));
	
	pReturn = PyObject_CallMethodObjArgs(pInstance, PyString_FromString("storeMessage"), storeMessage, 0);
	Py_DECREF(pReturn);
	Py_DECREF(storeMessage);


	// test read
	PyObject *readMessage;
	readMessage = PyList_New(clusterCount);
	for (int i = 0; i< clusterCount; i++)
		PyList_SetItem(readMessage, i, PyInt_FromLong(i));
	
	Py_INCREF(Py_None);
	PyList_SetItem(readMessage, 8, Py_None);
	
	pReturn = PyObject_CallMethodObjArgs(pInstance, PyString_FromString("readMessage"), readMessage, PyInt_FromLong(1), 0);

	printf("iterations: %d\n", PyInt_AsLong(pReturn));
	
	Py_DECREF(readMessage);
	Py_DECREF(pReturn);
	
	if (PyErr_Occurred())
		PyErr_Print();
	
	
	Py_Finalize();
	return 0;
}
