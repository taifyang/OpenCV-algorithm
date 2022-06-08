#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	开运算
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void open(cv::Mat& src, cv::Mat& dst, cv::Size size)
{
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, size);
	cv::erode(src, dst, element);
	cv::dilate(dst, dst, element);
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;

	open(src, dst, cv::Size(3, 3));
	cv::imwrite("open.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

