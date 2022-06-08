#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	计算梯度
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void gradient(cv::Mat& src, cv::Mat& dst, cv::Size size)
{
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, size), dst1, dst2;
	cv::dilate(src, dst1, element);
	cv::erode(src, dst2, element);
	dst = dst1 - dst2;
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	gradient(src, dst, cv::Size(3, 3));
    cv::imwrite("gradient.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

