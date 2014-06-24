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

//getting the classnames 
void classname_extract(){
	string line;
	ifstream myfile ("../classname.txt");
	if(myfile.is_open()){		
			myfile>>classname_1;
			myfile>>classname_2;		
	}
}

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
int counts=0;
ofstream myfile;

int filename_extract(){
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
		cout<<"** usage : \n./SVM_test_folder [TEST IMAGE DATASET FOLDER PATH]"<<endl;
		cout<<"The TEST IMAGE DATASET folder must have images in jpg format and must be named in the following pattern \"0001.jpg\",\"0010.jpg\""<<endl;
		cout<<"try again **"<<endl;
		return -1;
	}
	
	folder_path = argv[1];
	
	// prompt for saving the results to results.txt
	char answer[5];
	cout<<" Do you want to export the results ? \n The results will be stored in a file result.txt \n Press Y to store the results or N if you don't want to store the results :"<<endl;
	cin>>answer;
	
	if(answer[0] != 'Y'  && answer[0] != 'N' && answer[0] != 'y' && answer[0] != 'n'){
		cout<<"You responded with "<<answer<<endl;
		cout<<"Invalid response \n Program ending \n Try again"<<endl;
		return 0;
	}
	
	if(answer[0] == 'Y' || answer[0] == 'y'){
	myfile.open("results.txt");		
}
	
	CvSVM svm;
	svm.load("/home/vikiboy/bagofwords/3.SVM trainer/SVMdata.yml");
	cout<<"SVM data loaded"<<endl;
	
	Mat dictionary;
	FileStorage fs("/home/vikiboy/bagofwords/1.Vocabulary Builder/vocabulary_color_1000.yml",FileStorage::READ);
	fs["vocabulary"]>>dictionary;
	fs.release();
	
	
	
	Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SurfFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SurfDescriptorExtractor);   
    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);

for(i=1;;i++){
	filename_extract();
	int check = filename_extract();
	if(check == -1){
		break;
	}
	
	Mat img_test = source.clone();
    vector<KeyPoint> keypoints;        
    detector->detect(img_test,keypoints);
    Mat bowDescriptor; 
    bowDE.compute(img_test,keypoints,bowDescriptor);
    cout<<"Descriptors computed"<<endl;
	cout<<"No. of rows :"<<bowDescriptor.rows<<endl;
	cout<<"No. of columns :"<<bowDescriptor.cols<<endl;
	
		
		
	cout<<"SVM prediction #"<<i<<endl;
	cout<<"Image : "<<filename<<endl;
	float response = svm.predict(bowDescriptor);
	
	classname_extract();
	if(response==1){
		cout<<classname_1<<endl;
		counts++;
	}
	else if (response==2){
		cout<<classname_2<<endl;
	}
	
	if(answer[0] == 'Y' || answer[0] == 'y'){
		
		myfile<<"SVM prediction #"<<i<<endl;
		myfile<<"Image : "<<filename<<endl;
		
		if (response==1){
			myfile<<classname_1<<endl;
		//counts++;
		}
		else if (response==2){
			myfile<<classname_2<<endl;
		}
	}
	
	cout<<endl;
}
	
	myfile<<classname_1<<" :"<<endl;
	myfile<<counts<<" out of "<<total_images<<" total images"<<endl; 
	
	myfile<<classname_2<<" :"<<endl;
	myfile<<total_images-counts<<" out of "<<total_images<<" total images"<<endl; 
	myfile.close();
	return 0;
}
