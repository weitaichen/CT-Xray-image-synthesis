// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include "iostream"
#include "vector"
#include "string"
#include <sstream>
typedef unsigned char uchar;
using namespace cv;
using namespace std;

int main(int argc, const char *argv[])
{
	if(argc < 6){  //command example: main.exe, method, x_rotation_angle, y_rotation_angle, z_rotation_angle, slice_distance
		cout<<"command invalid, command should be: main.exe method x_rotation_angle y_rotation_angle z_rotation_angle slice_distance"<<endl;
		cout<<"command example: main.exe 1 90 0 0 10 "<<endl;
		return -1;
	}
	int method = stoi(string(argv[1])); //method==1:Compute slice of 2D image from CT data, method==2:integration all slice of 2D images to generate X-ray like 2D image from CT data
	if(method != 1 && method!=2){  
		cout<<"input method incorrect"<<endl;
		cout<<"method==1:Compute slice of 2D image from CT data"<<endl;
		cout<<"method==2:integration all slice of 2D images to generate X-ray like 2D image from CT data"<<endl;
		cout<<"command example: main.exe 1 90 0 0 10 "<<endl;
		return -1;
	}
	double x_angle=stod(string(argv[2])); //convert string to double, angle unit in degree
	double y_angle=stod(string(argv[3])); //x_angle, y_angle, z_angle indicate find a plane rotate along x, y, z axis which is parallel to x-y plane originally
	double z_angle=stod(string(argv[4])); 
	double dist_slice=stod(string(argv[5])); //translation distance from origin
	string imagesPath = "F:\\Program\\visual studio\\CT data processing\\x64\\Debug\\Fig\\*.png"; // load all images in folder where target images exist
	vector<String> fn;
	glob(imagesPath, fn, false); //search all images in imagePath and store file path as string in vector fn
	vector<Mat> vimage; 
	vector<double> vmean; 
	Mat img;
	for (size_t i=0; i<fn.size(); i++){
		img = imread(fn[i],0);
		img.convertTo(img, CV_64FC1) ; //image format change to double 
		vimage.push_back(img);
		Scalar mean,std; 
		cv::meanStdDev(img,mean,std);
		vmean.push_back(mean.val[0]); //store mean gray level of each input image
	}

	double mean=0;
	for(int k=0;k<vmean.size();k++)
		mean+=vmean[k];
	mean/=vmean.size(); //"mean" is mean of mean gray level of each input image
	for(int k=0;k<vimage.size();k++)
		vimage[k]=vimage[k]*mean/vmean[k]; //adjust mean gray level of each input image into "mean"
	cv::Point3d p;
	vector<cv::Point3d> vp;
	for(int i=0;i<img.rows;i++)
		for(int j=0;j<img.cols;j++){
			p=cv::Point3d(j,i,0); // points in 2D plane which parallel to x-y plane are stored into vector "vp"
			vp.push_back(p);
		}
		vector<cv::Point3d> vp_T;
		int h=vimage.size(); // "h" is number of input iamges, which represent depth in z direction
		Mat T1 = (Mat_<double>(4, 4) << 1, 0, 0, -1*img.cols/2, //translate 2D plane center to origin for later rotation operation
			0, 1, 0, -1*img.rows/2,
			0, 0, 1, 0,
			0, 0, 0, 1);
		Mat T2 = (Mat_<double>(4, 4) << 1, 0, 0, img.cols/2, //translate rotated 2D plane corner back to origin
			0, 1, 0, img.rows/2,
			0, 0, 1, 0,
			0, 0, 0, 1);
		double x_a=x_angle*3.14/180,z_a=z_angle*3.14/180,y_a=y_angle*3.14/180; //convert unit from degree to radian
		Mat R_X = (Mat_<double>(4, 4) << 1, 0, 0, 0, //rotation along x axis
			0, cos(x_a), -1*sin(x_a), 0,
			0, sin(x_a), cos(x_a), 0,
			0, 0, 0, 1);
		Mat R_Y = (Mat_<double>(4, 4) << cos(y_a), 0, sin(y_a), 0, //rotation along y axis
			0, 1, 0, 0,
			-1*sin(y_a), 0, cos(y_a), 0,
			0, 0, 0, 1);
		Mat R_Z = (Mat_<double>(4, 4) << cos(z_a), -1*sin(z_a), 0, 0, //rotation along z axis
			sin(z_a), cos(z_a), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		cv::Mat p_n(4, 1, CV_64F); 
		p_n.ptr<double>(0)[0]=0;
		p_n.ptr<double>(1)[0]=0;
		if(method==1)
			p_n.ptr<double>(2)[0]=dist_slice; //in method1: directly translate to input dist_slice to origin to extract 2D plane
		else if(method==2)
			p_n.ptr<double>(2)[0]=1; //in method2: translate 2D plane step by step and write all slice image for later image integration
		p_n.ptr<double>(3)[0]=1;
		p_n=R_X*p_n;
		p_n=R_Y*p_n;
		p_n=R_Z*p_n;
		Mat T3 = (Mat_<double>(4, 4) << 1, 0, 0, p_n.ptr<double>(0)[0], //Trnaslate 2D plane in plane vertical direction
			0, 1, 0, p_n.ptr<double>(1)[0],
			0, 0, 1, p_n.ptr<double>(2)[0],
			0, 0, 0, 1);
		vector <cv::Mat> v_my_mat;
		for(int k=0;k<vp.size();k++){
			cv::Mat my_mat(4, 1, CV_64FC1); 
			my_mat.ptr<double>(0)[0]=vp[k].x;
			my_mat.ptr<double>(1)[0]=vp[k].y;
			my_mat.ptr<double>(2)[0]=vp[k].z;
			my_mat.ptr<double>(3)[0]=1;
			my_mat=T1*my_mat; //translate 2D plane center to origin for later rotation operation
			my_mat=R_X*my_mat; //rotation along x axis
			my_mat=R_Y*my_mat; //rotation along y axis
			my_mat=R_Z*my_mat; //rotation along z axis
			my_mat=T2*my_mat; //translate rotated 2D plane corner back to origin
			v_my_mat.push_back(my_mat);
		}
		int step;
		vector<cv::Mat> voutput;
		for(step=1;step<h;step++){
			cout<<step<<endl;
			for(int k=0;k<v_my_mat.size();k++){
				v_my_mat[k]=T3*v_my_mat[k]; //Trnaslate 2D plane in plane vertical direction
				cv::Mat temp=v_my_mat[k];
				vp_T.push_back(cv::Point3d(temp.at<double>(0,0),temp.at<double>(1,0),temp.at<double>(2,0)));
			}
			vector<double> vd;
			cv::Mat map=cv::Mat(img.rows,img.cols,CV_64FC1,cv::Scalar(0));
			//-----------------------trilinear filter to correctly sample data--------------------
			for(int k=0;k<vp_T.size();k++){
				double temp_x=vp_T[k].x;
				double temp_y=vp_T[k].y;
				double temp_z=vp_T[k].z;
				int flag=0;
				if(temp_x<0) //if 2D plane outbound data space, then store 0 into array
					flag=1;
				if(temp_x>img.cols-2)
					flag=1;
				if(temp_y<0)
					flag=1;
				if(temp_y>img.rows-2)
					flag=1;
				if(temp_z<0)
					flag=1;
				if(temp_z>h-2)
					flag=1;
				if(flag==0){
					double x1=int(temp_x);
					double x2=int(temp_x)+1;
					double y1=int(temp_y);
					double y2=int(temp_y)+1;
					double z1=int(temp_z);
					double z2=int(temp_z)+1;
					if(x1 != x2 && y1 != y2 && z1!=z2){ //trilinear filter algorithm
						double inter_y1=vimage[int(z1)].ptr<double>(int(y1))[int(x1)]*(x2-temp_x)/(x2-x1)+vimage[int(z1)].ptr<double>(int(y1))[int(x2)]*(temp_x-x1)/(x2-x1);
						double inter_y2=vimage[int(z1)].ptr<double>(int(y2))[int(x1)]*(x2-temp_x)/(x2-x1)+vimage[int(z1)].ptr<double>(int(y2))[int(x2)]*(temp_x-x1)/(x2-x1);
						double inter_z1=inter_y1*(y2-temp_y)/(y2-y1)+inter_y2*(temp_y-y1)/(y2-y1);
						inter_y1=vimage[int(z2)].ptr<double>(int(y1))[int(x1)]*(x2-temp_x)/(x2-x1)+vimage[int(z2)].ptr<double>(int(y1))[int(x2)]*(temp_x-x1)/(x2-x1);
						inter_y2=vimage[int(z2)].ptr<double>(int(y2))[int(x1)]*(x2-temp_x)/(x2-x1)+vimage[int(z2)].ptr<double>(int(y2))[int(x2)]*(temp_x-x1)/(x2-x1);
						double inter_z2=inter_y1*(y2-temp_y)/(y2-y1)+inter_y2*(temp_y-y1)/(y2-y1);
						double trilinear=inter_z1*(z2-temp_z)/(z2-z1)+inter_z2*(temp_z-z1)/(z2-z1);

						vd.push_back(trilinear);
					}
					else{
						double temp=vimage[int(temp_z)].ptr<double>(int(temp_y))[int(temp_x)];
						vd.push_back(temp);
					}
				}
				else
					vd.push_back(0); //vd store gray level of each point in 2D plane
			}

			//---------------------show result----------------
			int count=0;
			for(int i=0;i<img.rows;i++)
				for(int j=0;j<img.cols;j++){
					map.ptr<double>(i)[j]=vd[count];
					count++;
				}
				voutput.push_back(map);
				if(method==1){
					cv::normalize(map, map, 0, 255,NORM_MINMAX);
					map.convertTo(map, CV_8UC1); //image format change to unsigned 8bit/pixel 
					cv::imshow("CT slice image",map);
					cv::imwrite("CT slice image.png",map); //write an image in main.exe folder
					waitKey();
					return 0;
				}
				else if(method==2){
					stringstream ss; 
					ss << step;
					string str = ss.str(); //convert int to string
					str=str+".png";
					cv::imwrite(str,map); //write multiple images in main.exe folder
				}
				vp_T.clear();
				vd.clear();
		}
		cv::Mat result=cv::Mat(img.rows,img.cols,CV_64F,cv::Scalar(0));
		for(int i=0;i<img.rows;i++) //in method2: find average image of all slice image in same rotation angle
			for(int j=0;j<img.cols;j++){
				double count=0;
				for(int k=0;k<voutput.size();k++){
					result.at<double>(i,j)+=voutput[k].at<double>(i,j);
					count++;
				}
				result.at<double>(i,j)/=count;
			}
			cv::normalize(result, result, 0, 255,NORM_MINMAX);
			result.convertTo(result, CV_8UC1);
			cv::imwrite("CT integration image.png",result);
			cv::imshow("CT integration image",result);
			vimage.clear();
			vmean.clear();

			waitKey();
			return 0;
}

