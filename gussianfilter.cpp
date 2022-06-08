#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	高斯滤波
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 * @param sigmaX	x方向方差
 * @param sigmaY	y方向方差
 */
void gaussianfilter(cv::Mat& src, cv::Mat& dst, cv::Size size, double sigmaX, double sigmaY)
{
    cv::Mat kernel = cv::Mat::zeros(size, CV_64F);
	int h = (size.height - 1) / 2, w = (size.width - 1) / 2;
	double sum = 0.0;
	for (int i = 0; i < size.height; ++i)
	{
		for (int j = 0; j < size.width; ++j)
		{
			kernel.at<double>(i, j) = exp(-(pow(i - h, 2) + pow(j - w, 2)) / (2 * sigmaX * sigmaY));
			sum += kernel.at<double>(i, j);
		}
	}
	for (int i = 0; i < size.height; ++i)
	{
		for (int j = 0; j < size.width; ++j)
		{
			kernel.at<double>(i, j) /= sum;
		}
	}
	cv::filter2D(src, dst, src.depth(), kernel);
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

    gaussianfilter(src, dst, cv::Size(13, 13), 1, 1);
    cv::imwrite("gaussianfilter.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

