#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <fstream>


using namespace cv;
using namespace std;

int class_1,class_2;

int main(int argc,char* argv[]){
	
	// Reads the Histograms 

	Mat svm_training_data;
	FileStorage fs("../2.Histograms Calculator/SVM_training_data.yml",FileStorage::READ);
	fs["SVM_training_data"]>>svm_training_data;
	fs.release();
	cout<<"SVM training data acquired"<<endl;
	//cout<<svm_training_data<<endl;
	
	// For assigning the labels 
	Mat training_data_labels(svm_training_data.rows,1,CV_32SC1);
	
	class_2 = svm_training_data.rows -1; //The index of the last row 
	//cout<<"No. of rows :"<<training_data_labels.rows<<endl;
	//cout<<"No. of cols :"<<training_data_labels.cols<<endl;
	
	string classname_1,classname_2;
	cout<<"What's the 1st class's name ?"<<endl;
	getline(cin,classname_1);
	
	cout<<"What's the 2nd class's name ?"<<endl;
	getline(cin,classname_2);
	
	cout<<"How many objects belong to class : "<<classname_1<<endl;
	cin>>class_1;
	
	cout<<"No. of objects in "<<classname_2<< " : "<<class_2-class_1+1<<endl;
	
	// Assigning the labels 
	training_data_labels.rowRange(0,(class_1)-1).setTo(1);
	training_data_labels.rowRange((class_1),class_2).setTo(2);
	
	//cout<<"Printing labels"<<endl;
	//cout<<training_data_labels<<endl;
	
	cout<<"SVM Label created "<<endl<<" Response 1 : for "<<classname_1 <<endl<<" Response 2 : for "<<classname_2 <<endl;
	
	// SVM parameters ( studying up about these :P )
	
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::RBF;
	
	
	cout<<"SVM training started.."<<endl;
	CvSVM svm;
	svm.train(svm_training_data,training_data_labels,Mat(),Mat(),params);
	
	cout<<"SVM training finished"<<endl;
	
	// exporting the SVM data 
	svm.save("SVMdata.yml");
	
	// exporting the SVM class names 
	ofstream myfile;
	myfile.open("../4.SVM tester/classname.txt");
	myfile<<classname_1<<endl;
	myfile<<classname_2<<endl;
	myfile.close();	
	return 0;	
}
