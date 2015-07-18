#include <cv.h>  
#include <highgui.h>   
#include <string>
#include<fstream>

using namespace cv;

cv::Mat src;
cv::Mat dst;

const cv::Size Img_Size(640, 480);

cv::Point2i left_bottom;
cv::Point2i left_top;

cv::Point2i right_bottom;
cv::Point2i right_top;

int num_lines = 0;

void on_mouse(int event, int x, int y, int flags, void* ustc)
{
	if (num_lines < 2){
		static CvPoint pre_pt = { -1, -1 };
		static CvPoint cur_pt = { -1, -1 };
		if (event == CV_EVENT_LBUTTONDOWN)
		{
			dst.copyTo(src);
			pre_pt = cvPoint(x, y);
			cv::circle(src, pre_pt, 3, cvScalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
			imshow("src", src);
			src.copyTo(dst);
		}
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
		{
			dst.copyTo(src);
			cur_pt = cvPoint(x, y);
			cv::line(src, pre_pt, cur_pt, cvScalar(0, 255, 0, 0), 1, CV_AA, 0);
			imshow("src", src);
		}
		else if (event == CV_EVENT_LBUTTONUP)
		{
			dst.copyTo(src);
			cur_pt = cvPoint(x, y);
			cv::circle(src, cur_pt, 3, cvScalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
			cv::line(src, pre_pt, cur_pt, cvScalar(0, 255, 0, 0), 1, CV_AA, 0);
			if (pre_pt.x < Img_Size.width / 2){
				if (pre_pt.y < Img_Size.height / 2){
					left_top = pre_pt;
					left_bottom = cur_pt;
				}
				else{
					left_bottom = pre_pt;
					left_top = cur_pt;
				}
			}
			else{
				if (pre_pt.y < Img_Size.height / 2){
					right_top = pre_pt;
					right_bottom = cur_pt;
				}
				else{
					right_bottom = pre_pt;
					right_top = cur_pt;
				}
			}
			imshow("src", src);
			src.copyTo(dst);
			++num_lines;
		}
	}
}

int main()
{
	string filename="Video_14_zm_640x480.AVI";
	//std::cout << "input file name:" << std::endl;
	//std::cin  >> filename;
	Mat src_gray;

	std::ofstream output;
	output.open(filename+".txt");
	VideoCapture vidCapture(filename);

	Mat frame;

	namedWindow("src", CV_WINDOW_AUTOSIZE);
	namedWindow("d", CV_WINDOW_AUTOSIZE);
	
	vidCapture >> src;
	src.copyTo(dst);

	imshow("src", src);
	cv::setMouseCallback("src", on_mouse, 0);
	
	
	//for (;;){
	while (!src.empty()){

		if (num_lines<2){ waitKey(66); }
		else{

			double l = (right_bottom.x + right_top.x) / 2 - (left_bottom.x + left_top.x) / 2;

			vidCapture >> src;
			cv::line(src, left_bottom, left_top, cvScalar(0, 255, 0, 0), 1, CV_AA, 0);
			cv::line(src, right_bottom, right_top, cvScalar(0, 255, 0, 0), 1, CV_AA, 0);
			//cvtColor(src, src_gray, COLOR_RGB2GRAY);

			

			cvtColor(src, src_gray, COLOR_RGB2GRAY);

			bitwise_xor(Scalar(255, 0, 0, 0), src_gray, src_gray);//xor,颜色取反

			GaussianBlur(src_gray, src_gray, Size(5, 5), 0, 0);

			threshold(src_gray, src_gray, 200, 255, 0);//阈值分割
			imshow("d", src_gray);
			

			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;
			findContours(src_gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			for (size_t i = 0; i < contours.size(); ++i)
			{
				Moments m = moments(contours[i]);
				Point center(m.m10 / m.m00, m.m01 / m.m00);//计算鱼的中心点
				if (center.x>(left_bottom.x + left_top.x) / 2 && center.x < (right_bottom.x + right_top.x) / 2){
					cv::circle(src, center, 3, cvScalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
					//std::cout << center.x << " " << center.y << std::endl;
					double p= (center.x - (left_bottom.x + left_top.x) / 2) / l;
					output << p << std::endl;
				}
			}
			imshow("src", src);
			int key = waitKey(6);
			if (key == 27){ break; }
		}
	}
	output.close();
	return 0;
}