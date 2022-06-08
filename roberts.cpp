#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	roberts算子
 * @param src		输入图像
 * @param dst		输出图像
 */
void roberts(cv::Mat& src, cv::Mat& dst)
{	
	cv::Mat oper1 = (cv::Mat_<float>(2, 2) << 0, 1, -1, 0); 
	cv::Mat oper2 = (cv::Mat_<float>(2, 2) << 1, 0, -1, 0); 
	cv::flip(oper1, oper1, -1);
	cv::flip(oper2, oper2, -1);
	cv::Mat dst1, dst2;

	cv::filter2D(src, dst1, src.depth(), oper1); 
	cv::filter2D(src, dst2, src.depth(), oper2);

	cv::convertScaleAbs(dst1, dst1);
	cv::convertScaleAbs(dst2, dst2);
	dst = dst1 + dst2;
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	roberts(src, dst);
    cv::imwrite("roberts.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
