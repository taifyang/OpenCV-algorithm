#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:    DOG算子
 * @param src       输入图像
 * @param dst       输出图像
 * @param size      掩膜大小
 * @param sigma     方差
 * @param k         系数
 */
void DOG(cv::Mat& src, cv::Mat& dst, cv::Size size, double sigma, double k = 1.6)
{
    cv::Mat gaussian_dst1, gaussian_dst2;
    cv::GaussianBlur(src, gaussian_dst1, size, k*sigma);
    cv::GaussianBlur(src, gaussian_dst2, size, sigma);
	dst = gaussian_dst1 - gaussian_dst2;
    cv::threshold(dst, dst, 0, 255, cv::THRESH_BINARY);
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	DOG(src, dst, cv::Size(13, 13), 1);
    cv::imwrite("dog.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

