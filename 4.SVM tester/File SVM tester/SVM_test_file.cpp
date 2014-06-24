#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <fstream>

using namespace cv;
using namespace std;


string classname_1,classname_2;

void classname_extract(){
	string line;
	ifstream myfile ("/home/vikiboy/bagofwords/4.SVM tester/classname.txt");
	if(myfile.is_open()){		
			myfile>>classname_1;
			myfile>>classname_2;		
	}
}
	

int main(int argc,char* argv[]){
	if(argc!=2){
		cout<<"** usage : \n./SVM_test_file [TEST IMAGE FILEPATH]"<<endl;
		
		cout<<"try again **"<<endl;
		return -1;
	}
	
	CvSVM svm;
	svm.load("/home/vikiboy/bagofwords/3.SVM trainer/SVMdata.yml");
	cout<<"SVM data loaded"<<endl;
	
	Mat dictionary;
	FileStorage fs("/home/vikiboy/bagofwords/1.Vocabulary Builder/vocabulary_color_1000.yml",FileStorage::READ);
	fs["vocabulary"]>>dictionary;
	fs.release();
	
	Mat img_test = imread(argv[1],CV_LOAD_IMAGE_COLOR);
	
	Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SurfFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SurfDescriptorExtractor);   
    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);


    vector<KeyPoint> keypoints;        
    detector->detect(img_test,keypoints);
    Mat bowDescriptor; 
    bowDE.compute(img_test,keypoints,bowDescriptor);
    cout<<"Histogram computed"<<endl;
	cout<<"No. of rows :"<<bowDescriptor.rows<<endl;
	cout<<"No. of columns :"<<bowDescriptor.cols<<endl;
	
	cout<<"SVM prediction"<<endl;
	float response = svm.predict(bowDescriptor);
	classname_extract();
	
	if(response==1){
		cout<<classname_1<<endl;
	}
	else if (response==2){
		cout<<classname_2<<endl;
	}
	
	return 0;
}
