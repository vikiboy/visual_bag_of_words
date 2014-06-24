#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/core/core.hpp"
#include <stdio.h>
#include <cstring>

using namespace cv;
using namespace std;

char* filename = new char[100];
string window_result = "Keypoints";
Mat source,img_keypoints;
int i;
string folder_path;
char number[1024];
char file_path[1024];
Mat img;
string zeroes;
string final;
float total_images;

int filename_extract(){
	
	// This function is to extract the filenames from the IMAGE DATASET
	// folder which will be stored in the global char variable filename
	// and will be subsequently used to load the image
	// the files should be of the following format 
	// 0001.jpg, 0010.jpg, 0100.jpg, 1000.jpg
	// This also helps in detecting whether an image exists or not of the name in use in the loop
	// This also helps in calculating the total no. of images once run in the loop
	
	if(i>0&&i<10){
		zeroes = "000";
	}
	else if(i>=10&&i<100){
		zeroes = "00";
	}
	else if(i>=100&&i<=999){
		zeroes = "0";
	}
	else {
		zeroes = "" ;
	}
	
	
	stringstream ss;
	ss << i;
	string suffix = ss.str();
	if(!zeroes.empty()){
		final = zeroes + suffix ;
		//cout<<" Not empty"<<endl;
	}
	else if(zeroes.empty()){
		final = suffix;
		//cout<<" Empty"<<endl;
	}
	strcpy(number, final.c_str()); 
	strcpy(file_path, folder_path.c_str()); 
	sprintf(filename,"%s%s.jpg",file_path,number);
	source = imread(filename,CV_LOAD_IMAGE_COLOR);

	if(source.empty()){
		total_images = i-1;	
		cout<<"There are "<<total_images<<" images in the folder"<<endl;		
		return -1;
	}
}

int main(int argc,char* argv[]){
	if(argc!=2){
		cout<<"** usage : \n./vocabulary_builder [IMAGE DATASET FOLDER PATH]"<<endl;
		cout<<"The IMAGE DATASET folder must have images in jpg format and must be named in the following pattern \"0001.jpg\",\"0010.jpg\""<<endl;
		cout<<"try again **"<<endl;
		return -1;
	}
	// The path to the image dataset folder 
	folder_path = argv[1];	
	
	//SURF detectors and descriptors created
	Ptr<FeatureDetector> detector(new SurfFeatureDetector(400));
    Ptr<DescriptorExtractor> extractor(new SurfDescriptorExtractor);    
	vector<KeyPoint> keypoints;
	
	
	Mat descriptors; // to hold the descriptor values of the currently used image
	
	// To hold the descriptor values of all the images 
	// the Matrix descriptors will be stacked in the Matrix training_descriptors 
	
	Mat training_descriptors;//(1,extractor->descriptorSize(),extractor->descriptorType());
	
	//Parameters for the Bag of Words trainer based on KMeans clustering
	int dictionarySize = 200; //no. of bags being made 		
	TermCriteria tc(CV_TERMCRIT_ITER,100,0.001); // type of iterations, no. of iterations, accuracy		
	int retries = 1; 		
	int flags = KMEANS_PP_CENTERS;	
	
	// To count the total no. of images 	
	for(i=1;;i++){
		filename_extract();		
		int response = filename_extract();
		if(response == -1){
			break;
		}
	}	
	
	// To load all the images 
	for(i=1;;++i){
		filename_extract();		// gets the filename from the folder 

		// To check whether the current image exists or not 
		// if not the programs ends
		int response = filename_extract();
		if(response == -1){
			break;
		}
				
		cout<<filename<<endl; //prints the image being used 

		
		detector->detect(source,keypoints);	// Keypoints detected
		extractor->compute(source,keypoints,descriptors); // Descriptors computed
		training_descriptors.push_back(descriptors); // Being added the training_descriptors alongside all the previously added descriptors from previous images
		cout<<(i/total_images)*100.0<<" percent done"<<endl;
		cout<<"Image no. : "<<i<<" Descriptor Size : "<<descriptors.rows<<endl;
	}
	
	//Exporting the training_descriptors file 
	cout << "Total descriptors: " << training_descriptors.rows << endl;
	FileStorage fs("training_descriptors.yml", FileStorage::WRITE);
	fs << "training_descriptors" << training_descriptors;
	fs.release();
    
	//Constructing the vocabulary     
    BOWKMeansTrainer bowtrainer(dictionarySize,tc,retries,flags);
    bowtrainer.add(training_descriptors);
    Mat vocabulary = bowtrainer.cluster();
	
	//Exporting the vocabulary 
    cout << "cluster BOW features" << endl;
    FileStorage fs1("vocabulary_color_1000.yml", FileStorage::WRITE);
	fs1 << "vocabulary" << vocabulary;
	fs1.release();
	cout<<" Vocabulary constructed"<<endl;
    /*
	drawKeypoints(source,keypoints,img_keypoints,Scalar::all(-1),DrawMatchesFlags::DEFAULT);
	namedWindow("Original",CV_WINDOW_AUTOSIZE);
	namedWindow(window_result,CV_WINDOW_AUTOSIZE);
	imshow("Original",source);
	imshow(window_result,img_keypoints);
	waitKey(0);
	destroyAllWindows();	*/
	return 0;
}


