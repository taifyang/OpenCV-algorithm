#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description: 	黑帽运算
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void blackhat(cv::Mat& src, cv::Mat& dst, cv::Size size)
{
	cv::Mat element = getStructuringElement(cv::MORPH_RECT, size);
	cv::morphologyEx(src, dst, cv::MORPH_CLOSE, element);
	dst = dst - src;
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	blackhat(src, dst, cv::Size(3, 3));
	cv::imwrite("blackhat.jpg", dst);
	
	system("pause");
	return EXIT_SUCCESS;
}

