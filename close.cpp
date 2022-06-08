#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	闭运算
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void close(cv::Mat& src, cv::Mat& dst, cv::Size size)
{
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, size);
	cv::dilate(src, dst, element);
	cv::erode(dst, dst, element);
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	close(src, dst, cv::Size(3, 3));
	imwrite("close.jpg", dst);
	
	system("pause");
	return EXIT_SUCCESS;
}

