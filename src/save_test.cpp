#include <iostream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

int size_sample=30;

int main(int argc, char** argv) {
	std::ifstream positiveFile;
	positiveFile.open("positiveSamples.txt");

	cv::Mat img = cv::Mat(size_sample,size_sample, CV_8UC3, cv::Scalar(0,0,0));
	
	cv::namedWindow("Image");
	
	while(true){
		for(int i=0;i<size_sample;i++){
			for(int j=0;j<size_sample;j++){
				int a,b,c;
				positiveFile>>a;
				positiveFile>>b;
				positiveFile>>c;
				
				img.at<cv::Vec3b>(i,j)[0]=(uchar)a;
				img.at<cv::Vec3b>(i,j)[1]=(uchar)b;
				img.at<cv::Vec3b>(i,j)[2]=(uchar)c;
			}
		}
		cv::imshow("Image",img);
		cv::waitKey(0);
	}
	cv::destroyWindow("Image");
    return 0;
}
