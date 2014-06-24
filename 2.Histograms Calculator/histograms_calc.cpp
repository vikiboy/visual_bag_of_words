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

char temp[1024];

Mat svm_training_data;//(1500,200,CV_32FC1);

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
int total_images;

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
	strcpy(temp,number);
		
	if(source.empty()){
		total_images = i-1;	
		cout<<"There are "<<total_images<<" images in the folder"<<endl;		
		return -1;
	}
}


int main(int argc,char* argv[]){
	if(argc!=2){
		cout<<"** usage : \n./histogram_calc [IMAGE DATASET FOLDER PATH]"<<endl;
		cout<<"The IMAGE DATASET folder must have images in jpg format and must be named in the following pattern \"0001.jpg\",\"0010.jpg\""<<endl;
		cout<<"try again **"<<endl;
		return -1;
	}
	
	// setting the IMAGE DATASET FOLDER for the training set 	
	folder_path=argv[1];

	
	cout<<"Training data sets"<<endl;
	
	// importing the dictionary from the file created in the vocabulary_builder.cpp
	Mat dictionary;
	FileStorage fs("../1.Vocabulary Builder/vocabulary_color_1000.yml",FileStorage::READ);
	fs["vocabulary"]>>dictionary;
	fs.release();
	
	// Creating the matcher, detector, descriptor and BOWImgDescriptorExtractor for creating the histograms
    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SurfFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SurfDescriptorExtractor);    
    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);
	
	
	cout<<"Vocabulary set created"<<endl;
	cout<<"Image Training started"<<endl;
	
	
	//To store the image file name
    char * filename_1 = new char[100];
    //To store the image tag name - only for save the descriptor in a file
    char * imageTag = new char[10];
 
	// To load all the images in the folder

    for(i=1;;i++){
    
    
    filename_extract(); // gets the filename from the folder 
    
    // To check whether the current image exists or not 
	// if not the programs ends
	
    int response = filename_extract();
	if(response == -1){
		break;
	}
    
    // saving the filenames for the histogram files to be exported 
    if(i<1000){
		sprintf(filename_1,"./Histograms/%s.yml",temp);
		cout<<"Filename 1 :"<<filename_1<<endl;
	}
	
	if(i>=1000){
		sprintf(filename_1,"./Histograms/%d.yml",i);
		cout<<"Filename 1 :"<<filename_1<<endl;
	}
	
	//exporting the histograms stacked for SVM training 
    FileStorage fs1(filename_1, FileStorage::WRITE);    
    FileStorage fs2("SVM_training_data.yml", FileStorage::WRITE);    
    
    //read the image
    Mat img=imread(filename,CV_LOAD_IMAGE_COLOR);        
    
    //To store the keypoints that will be extracted by SURF
    vector<KeyPoint> keypoints;        
    //Detect SURF keypoints (or feature points)
    detector->detect(img,keypoints);
    //To store the BoW (or BoF) representation of the image
    Mat bowDescriptor;        
    //extract BoW (or BoF) descriptor from given image
    bowDE.compute(img,keypoints,bowDescriptor);
    /*for (int l = 0; l < bowDescriptor.cols; l++){
    cout << bowDescriptor.at<float>(0, l) << ",";
	}*/
    
    cout<<"Descriptors computed"<<endl;
	svm_training_data.push_back(bowDescriptor); // all histograms in one matrix
	cout<<"File added to svm_training_data"<<endl;
    //prepare the yml (some what similar to xml) file
    sprintf(imageTag,"img%d",i);            
    //write the new BoF descriptor to the file
    fs1 << imageTag << bowDescriptor;        
	fs2 << "SVM_training_data "<<svm_training_data;
    //You may use this descriptor for classifying the image.
    cout<<"Files written"<<endl;        
    //release the file storage
    fs1.release();
    fs2.release();
}
	
	cout<<"Histograms computed and SVM training data computed"<<endl;
	return 0;
}
