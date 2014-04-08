#include "libsvm/svm.h"
#include "libsvm/svmWrapper.hpp"

void exit_with_help()
{
	printf(
	"Usage: svm-predict test_file model_file <optional := output_file>\n"
	);
	exit(1);
}

/*************************************************************/
/*****************         MAIN         **********************/
/*************************************************************/

int main(int _argc, char *_argv[]) {
	SVM svm;
	int errorNo = svm.parseInput( _argc, _argv );
	if( errorNo == -1 ) {
		exit_with_help();
	}
	svm.init(30*30*3);
	svm.predictFromFile();
return 0;	
}
