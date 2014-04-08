#include <stdexcept>
#include <iostream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <sys/time.h>

#include "libsvm/svmWrapper.hpp"

char NORMAL[]= "\033[0m";
char RED[]= "\033[0;31m";
char BLUE[]= "\033[0;34m";
char WHITE[]= "\033[37;01m";

cv::Mat img;
cv::Mat imgAG;
SVM *svm;

int size_sample=30;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	cv::Mat imgRoi;
	std::vector<double> input;
	input.clear();
	struct timeval tvalBefore, tvalAfter;
	if  ( event == cv::EVENT_LBUTTONDOWN ){
		gettimeofday (&tvalBefore, NULL);
          imgRoi = imgAG(cv::Rect(x-size_sample/2, y-size_sample/2, size_sample, size_sample));
          int r=99;
          svm->predict(imgRoi, r);
          gettimeofday (&tvalAfter, NULL);
          cv::rectangle( img, cv::Point( x-size_sample/2, y-size_sample/2 ), cv::Point( x+size_sample/2, y+size_sample/2 ), cv::Scalar( 255, 255, 255 ));
          double timeElapsed = tvalAfter.tv_sec+(tvalAfter.tv_usec/1000000.0) - (tvalBefore.tv_sec+(tvalBefore.tv_usec/1000000.0));
          if(r == 1 ){
			  std::cout << BLUE << "Positive Sample! : "<< 1./timeElapsed << NORMAL << std::endl;
		  }
		  else{
			  std::cout << RED << "Negative Sample! : "<< 1./timeElapsed << NORMAL << std::endl;
		  }
     }
}


void grassDetection(cv::Mat &image){
	cv::Mat imgRoi;
	std::vector<double> input;
	struct timeval tvalBefore, tvalAfter;
	gettimeofday (&tvalBefore, NULL);
	
	int cp=0, cn=0;
	for(int i=0;i<image.rows;i+=size_sample/2){
		for(int j=0;j<image.cols;j+=size_sample/2){
			if ( (image.rows-i) > size_sample && (image.cols-j) > size_sample ) {
				imgRoi = imgAG(cv::Rect(j, i, size_sample, size_sample));
				int r=99;
				svm->predict(imgRoi, r);
				if ( r == 1 ) {
					cp++;
//					std::cout << BLUE << "Positive sample - position (" << j << ", " << i << ")" << NORMAL << std::endl;
					cv::rectangle( img, cv::Point( j-size_sample/2, i-size_sample/2 ), cv::Point( j+size_sample/2, i+size_sample/2 ), cv::Scalar( 0, 0, 0 ),CV_FILLED);
				}
				else {
					cn++;
				}
			}
		}
	}
	gettimeofday (&tvalAfter, NULL);
	double timeElapsed = tvalAfter.tv_sec+(tvalAfter.tv_usec/1000000.0) - (tvalBefore.tv_sec+(tvalBefore.tv_usec/1000000.0));
	std::cout << WHITE << "FPS : "<< 1./timeElapsed << NORMAL << std::endl;
	std::cout << WHITE << "CP  : "<< cp << NORMAL << std::endl;
	std::cout << WHITE << "CN  : "<< cn << NORMAL << std::endl;
}

int main(int argc, char** argv) {
	if(argc<3){
		printf("Usage: <name> <image_file> <model> <optional:= sample_size=30\n");
		return -1;
	}
	
	if( argc == 4 ){
		int temp_size_sample = atoi(argv[3]);
		if( temp_size_sample>=5 && temp_size_sample<=30){
			size_sample = temp_size_sample;
		}
	}
	printf("Sample Size: %d\n",size_sample);
	
	img = cv::imread(argv[1]);
	cv::Mat result = img;
	imgAG = img.clone();
	
	
	if ( img.empty() ){ 
          std::cout << "Error loading the image" << std::endl;
          return -2; 
    }
	
	cv::namedWindow("Original Image");
	cv::namedWindow("ImageAG");
	svm = new SVM();
	svm->init(size_sample*size_sample*3);
	svm->loadModel(argv[2]);
	
	cv::setMouseCallback("Original Image", CallBackFunc, NULL);
	
	grassDetection(result);
	
	while(true){
		cv::imshow("ImageAG",imgAG);
		cv::imshow("Original Image",img);
		cv::waitKey(10);
	}
	delete(svm);
return 0;
}
