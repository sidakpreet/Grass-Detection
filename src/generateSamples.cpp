#include <image_transport/image_transport.h>

#include <ros/ros.h>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sensor_msgs/Image.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

char NORMAL[]= "\033[0m";
char RED[]= "\033[0;31m";
char BLUE[]= "\033[0;34m";
char WHITE[]= "\033[37;01m";

std::ofstream commonFile,commonFile2;

cv::Mat img;

std::string name;

int size_sample=30;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	int countP,countN;
	cv::Mat imgRoi;
     if  ( event == cv::EVENT_LBUTTONDOWN ){
          std::cout << BLUE << "Saving positive sample - position (" << x << ", " << y << ")" << NORMAL << std::endl;
          imgRoi = img(cv::Rect(x, y, size_sample, size_sample));
          commonFile2<<"+1 ";
          countP=0;
          for(int i=0;i<size_sample;i++){
			  for(int j=0;j<size_sample;j++){
				  commonFile<<(int)imgRoi.at<cv::Vec3b>(i,j)[0]<<"\t"
							  <<(int)imgRoi.at<cv::Vec3b>(i,j)[1]<<"\t"
							  <<(int)imgRoi.at<cv::Vec3b>(i,j)[2]<<"\t";
				  commonFile2<<countP++<<":"<<(int)imgRoi.at<cv::Vec3b>(i,j)[0]<<" ";
				  commonFile2<<countP++<<":"<<(int)imgRoi.at<cv::Vec3b>(i,j)[1]<<" ";
				  commonFile2<<countP++<<":"<<(int)imgRoi.at<cv::Vec3b>(i,j)[2]<<" ";
			  }
		  }
          commonFile<<"+1"<<std::endl;
          commonFile2<<countP++<<":-1"<<std::endl;
          cv::rectangle( img, cv::Point( x, y), cv::Point( x+size_sample, y+size_sample ), cv::Scalar( 255, 255, 255 ));
     }
     else if  ( event == cv::EVENT_RBUTTONDOWN ){
		 imgRoi = img(cv::Rect(x, y, size_sample, size_sample));
		 commonFile2<<"-1 ";
		 countN=0;
          for(int i=0;i<size_sample;i++){
			  for(int j=0;j<size_sample;j++){
				  commonFile<<(int)imgRoi.at<cv::Vec3b>(i,j)[0]<<"\t"
							  <<(int)imgRoi.at<cv::Vec3b>(i,j)[1]<<"\t"
							  <<(int)imgRoi.at<cv::Vec3b>(i,j)[2]<<"\t";
				commonFile2<<countN++<<":"<<(int)imgRoi.at<cv::Vec3b>(i,j)[0]<<" ";
				commonFile2<<countN++<<":"<<(int)imgRoi.at<cv::Vec3b>(i,j)[1]<<" ";
				commonFile2<<countN++<<":"<<(int)imgRoi.at<cv::Vec3b>(i,j)[2]<<" ";
			  }
		  }
          commonFile<<"-1"<<std::endl;
          commonFile2<<countN++<<":-1"<<std::endl;
          cv::rectangle( img, cv::Point( x, y), cv::Point( x+size_sample, y+size_sample ), cv::Scalar( 0, 0, 0 ));
          
          std::cout << RED << "Saving negative sample - position (" << x << ", " << y << ")" << NORMAL << std::endl;
     }
}


int main(int argc, char** argv) {
	if(argc<2){
		printf("Usage: <name> <image_file>\n");
		return -1;
	}
	commonFile.open("Samples.txt",std::fstream::app);
	commonFile2.open("Samples2.txt",std::fstream::app);
	img=cv::imread(argv[1]);
	if ( img.empty() ){ 
          std::cout << "Error loading the image" << std::endl;
          return -2; 
    }
	cv::namedWindow("Original Image");
	cv::setMouseCallback("Original Image", CallBackFunc, NULL);
	while(true){
		cv::imshow("Original Image",img);
		cv::waitKey(10);
	}
    return 0;
}
