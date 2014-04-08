#ifndef SVM_WRAPPER_HPP
#define SVM_WRAPPER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "libsvm/svm.h"

class SVM{
public:
	SVM();
	~SVM();
	int loadModel(std::string modelFile);
	int setTrainingData_fromFile(std::string filename, int inputDimension);
	int parseInput(int argc, char *argv[]);
	int init(int inputDimension);
	int predictFromFile();
	int predict(std::vector<double> input, int &output);
private:
	int storeOutput;
	std::vector<double> split(std::string work, char delim, int rep=0);
	struct svm_node *x;
	int max_nr_attr;
	struct svm_model* model;
	std::ifstream inputFile;
	std::ofstream outputFile;
};

#endif /* SVM_WRAPPER_HPP */
