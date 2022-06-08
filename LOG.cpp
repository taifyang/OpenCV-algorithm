#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	LOG算子
 * @param src		输入图像
 * @param dst		输出图像
 */
void LOG(cv::Mat& src, cv::Mat& dst)
{
	cv::Mat kernel = (cv::Mat_<float>(5, 5) << 0, 0 ,-1, 0, 0, 0, -1, -2, -1, 0, -1, -2, 16, -2, -1, 0, -1, -2, -1, 0, 0, 0, -1, 0, 0);
	cv::filter2D(src, dst, src.depth(), kernel);
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;

	LOG(src, dst);
	cv::imwrite("log.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

