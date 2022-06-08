#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	区域生长算法
 * @param src		输入图像
 * @param dst		输出图像
 * @param pt		种子点
 * @param threshold	阈值
 */
void regiongrow(cv::Mat& src, cv::Mat& dst, cv::Point pt, int threshold)
{
    cv::Point ptGrowing;							
	int srcval = src.at<uchar>(pt.y, pt.x);							
    dst = cv::Mat::zeros(src.size(), CV_8UC1);		
	int DIR[8][2] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 } }; 
	std::vector<cv::Point> ptGrow;						
	ptGrow.push_back(pt);						
	dst.at<uchar>(pt.y, pt.x) = 255;			

	while (!ptGrow.empty())						
	{
		pt = ptGrow.back();						
		ptGrow.pop_back();

		for (int i = 0; i < 8; ++i)
		{
			ptGrowing.x = pt.x + DIR[i][0];
			ptGrowing.y = pt.y + DIR[i][1];
			if (ptGrowing.x < 0 || ptGrowing.y < 0 || ptGrowing.x >=src.cols || ptGrowing.y >= src.rows)
				continue;

			if (dst.at<uchar>(ptGrowing.y, ptGrowing.x) == 0)			
			{
				int curval = src.at<uchar>(ptGrowing.y, ptGrowing.x);   
				if (abs(srcval - curval) < threshold)					
				{
					dst.at<uchar>(ptGrowing.y, ptGrowing.x) = 255;		
					ptGrow.push_back(ptGrowing);						
				}
			}

		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	regiongrow(src, dst, cv::Point(100, 100), 50);
    cv::imwrite("regiongrow.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
