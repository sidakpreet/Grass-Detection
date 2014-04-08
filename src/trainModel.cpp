/* NOT WORKING */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "libsvm/svm.h"

char NORMAL[]= "\033[0m";
char BLACK[]= "\033[0;30m";
char RED[]= "\033[0;31m";
char GREEN[]= "\033[0;32m";
char BROWN[]= "\033[0;33m";
char BLUE[]= "\033[0;34m";
char MAGENTA[]= "\033[0;35m";
char CYAN[]= "\033[0;36m";
char LIGHTGRAY[]= "\033[0;37m";
char YELLOW[]= "\033[0;33m";
char WHITE[]= "\033[37;01m";


#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

static char *_line = NULL;
static int max_line_len;

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}


static char* readline(FILE *input);

class SVM{
public:
	SVM();
	~SVM();
	void doCrossValidation();
	void calculateModel();
	int setTrainingData_fromFile(std::string filename, int inputDimension);
	int saveModel( std::string name );
	void read_problem(const char *filename);
	struct svm_parameter param;		
	struct svm_problem trainData;
private:
	std::vector<std::string> split(std::string work, char delim, int rep=0);
	struct svm_model *model;
	struct svm_node *x_space;
	int cross_validation;
	int nr_fold;
};

void exit_with_help()
{
	printf(
	"Usage: svm-train [options] training_set_file [model_file]\n"
	"options:\n"
	"-s show the values after storing in trainingData\n"
	);
	exit(1);
}


int main(int argc, char **argv)
{
	SVM svm;
	const char *error_msg;
	svm.setTrainingData_fromFile(std::string("Samples.txt"),30*30*3);
	error_msg = svm_check_parameter(&svm.trainData, &svm.param);
	svm.calculateModel();
	
	//svm.saveModel(std::string("Model"));
	
	return 0;
}


/* Memeber Function definitions for class SVM */

SVM::SVM(){
	param.svm_type = C_SVC;
	param.kernel_type = POLY;
	param.degree = 3;
	param.gamma = 0;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	
	cross_validation = 1;
	nr_fold = 2;
}


SVM::~SVM(){
	//svm_free_and_destroy_model(&model);
	svm_destroy_param(&param);
	free(trainData.y);
	free(trainData.x);
	free(x_space);
}


void SVM::calculateModel() {
	model = svm_train(&trainData,&param);
}


int SVM::saveModel( std::string name ) {
	if ( svm_save_model(name.c_str(), model) ) {
		fprintf(stderr, "Can't save model to file %s\n", name.c_str());
		return -1;
	}
return 0;
}

void SVM::doCrossValidation()
{
	int i;
	int total_correct = 0;
	double total_error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	double *target = Malloc(double,trainData.l);

	svm_cross_validation(&trainData,&param,nr_fold,target);
	if( param.svm_type == EPSILON_SVR || param.svm_type == NU_SVR ) {
		for (i=0;i<trainData.l;i++) {
			double y = trainData.y[i];
			double v = target[i];
			total_error += (v-y)*(v-y);
			sumv += v;
			sumy += y;
			sumvv += v*v;
			sumyy += y*y;
			sumvy += v*y;
		}
		printf("Cross Validation Mean squared error = %g\n",total_error/trainData.l);
		printf("Cross Validation Squared correlation coefficient = %g\n", ((trainData.l*sumvy-sumv*sumy)*(trainData.l*sumvy-sumv*sumy)) / ((trainData.l*sumvv-sumv*sumv)*(trainData.l*sumyy-sumy*sumy))	);
	}
	else {
		for(i=0;i<trainData.l;i++) {
			if(target[i] == trainData.y[i]) {
				++total_correct;
			}
		}
		printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/trainData.l);
	}
	free(target);
}

int SVM::setTrainingData_fromFile(std::string filename, int inputDimension){
	std::ifstream inputFile;
	inputFile.open(filename.c_str());
	if(!inputFile.is_open()){
		std::cout<<" The input file does not exist! "<<std::endl;
		return -1;
	}
	int elements, max_index, inst_max_index;
	trainData.l = 0;
	elements = 0;
	std::string line;
	std::vector<std::string> dataEntries;
	int lineNumber=0;
	
	while ( !inputFile.eof() ) {
		if(getline(inputFile,line)==NULL){
			break;
		}
		++trainData.l;
	}
	
	printf( "Training Data Length ---------------- %d\t\n",trainData.l);
	
	inputFile.close();
	inputFile.open(filename.c_str());
	
	trainData.y = Malloc(double,trainData.l);
	trainData.x = Malloc(struct svm_node *,trainData.l);
	x_space = Malloc(struct svm_node,(inputDimension+1)*trainData.l);
	
	int j=0;
	
	for (int i=0;i<trainData.l;i++) {
		dataEntries.clear();
		if(getline(inputFile,line)==NULL){
			printf( "Not able to get line\n");
			break;
		}
		lineNumber++;
		dataEntries = split(line, '\t');
		if( dataEntries.size() != (inputDimension+1) ) {
			printf( "dataEntries.size() ---------------- %d - Required: %d\t\n",(int)dataEntries.size(),(inputDimension+1));
			std::cout<<"Incorrect File Format at line "<<lineNumber<<"\n";
			return -1;
		}
		
		trainData.x[i] = &x_space[j];
		
		trainData.y[i] = atoi(dataEntries[inputDimension].c_str());
		
		
		for (int k=0;k<inputDimension;k++) {
			x_space[j].index = k;
			x_space[j].value = (double)atof(dataEntries[k].c_str());
			++j;
		}
		x_space[j++].index = -1;
	}
	if(param.gamma == 0 && inputDimension > 0) {
		param.gamma = 1.0/max_index;
	}
	/*for(int i=0;i<trainData.l;i++) {
		printf( "%sY - %d\t%s",WHITE,(int)trainData.y[i],NORMAL );
		for(int j=0;j<inputDimension+1;j++) {
			printf( "%d\t",(int)trainData.x[i][j].index);
		}
		printf( "\n" );
	}*/
	inputFile.close();
return 0;	
}

std::vector<std::string> SVM::split(std::string work, char delim, int rep) {
	std::vector<std::string> d_;
    if (!d_.empty()){
		 d_.clear();  // empty vector if necessary
	}
    std::string buf = "";
    int i = 0;
    while (i < work.length()) {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1) {
            d_.push_back(buf);
            buf = "";
        } 
        else if (buf.length() > 0) {
            d_.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        d_.push_back(buf);
return d_;
}

static char* readline(FILE *input) {
	int len;
	
	if(fgets(_line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(_line,'\n') == NULL)
	{
		max_line_len *= 2;
		_line = (char *) realloc(_line,max_line_len);
		len = (int) strlen(_line);
		if(fgets(_line+len,max_line_len-len,input) == NULL)
			break;
	}
	return _line;
}


void SVM::read_problem(const char *filename) {
	int elements, max_index, inst_max_index, i, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}

	trainData.l = 0;
	elements = 0;

	max_line_len = 1024;
	_line = Malloc(char,max_line_len);
	while(readline(fp)!=NULL)
	{
		char *p = strtok(_line," \t"); // label

		// features
		while(1)
		{
			p = strtok(NULL," \t");
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
			++elements;
		}
		++elements;
		++trainData.l;
	}
	rewind(fp);

	trainData.y = Malloc(double,trainData.l);
	trainData.x = Malloc(struct svm_node *,trainData.l);
	x_space = Malloc(struct svm_node,elements);

	max_index = 0;
	j=0;
	for(i=0;i<trainData.l;i++)
	{
		inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
		readline(fp);
		trainData.x[i] = &x_space[j];
		label = strtok(_line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		trainData.y[i] = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;
		x_space[j++].index = -1;
	}

	if(param.gamma == 0 && max_index > 0)
		param.gamma = 1.0/max_index;

	if(param.kernel_type == PRECOMPUTED)
		for(i=0;i<trainData.l;i++)
		{
			if (trainData.x[i][0].index != 0)
			{
				fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
				exit(1);
			}
			if ((int)trainData.x[i][0].value <= 0 || (int)trainData.x[i][0].value > max_index)
			{
				fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
				exit(1);
			}
		}

	fclose(fp);
}
