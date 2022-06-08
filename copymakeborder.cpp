#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	边界复制
 * @param src		输入图像
 * @param dst		输出图像
 * @param top		图像上沿
 * @param bottom	图像下沿
 * @param left		图像左沿
 * @param right		图像右沿
 */
void copymakeborder(cv::Mat& src, cv::Mat& dst, int top, int bottom, int left, int right)
{
    dst = cv::Mat::zeros(src.rows + top + bottom, src.cols + left + right, src.type());
	for (int i = top; i < src.rows + top; ++i)
	{
		for (int j = left; j < src.cols + left; ++j)
		{
			dst.at<uchar>(i, j) = src.at<uchar>(i - top, j - left);
		}
	}
	for (int i = top; i < src.rows + top; ++i)
	{
		for (int j = 0; j < left; ++j)
		{
			dst.at<uchar>(i, j) = dst.at<uchar>(i, 2 * left - j);
		}
	}
	for (int i = top; i < src.rows + top; ++i)
	{
		for (int j = src.cols + left; j < src.cols + left + right; ++j)
		{
			dst.at<uchar>(i, j) = dst.at<uchar>(i, 2 * (src.cols + left - 1) - j);
		}
	}
	for (int i = 0; i < top; ++i)
	{
		for (int j = 0; j < src.cols + left + right; ++j)
		{
			dst.at<uchar>(i, j) = dst.at<uchar>(2 * top - i, j);
		}
	}
	for (int i = src.rows; i < src.rows + top; ++i)
	{
		for (int j = 0; j < src.cols + left + right; ++j)
		{
			dst.at<uchar>(i, j) = dst.at<uchar>(2 * (src.rows + top - 1) - i, j);
		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	copymakeborder(src, dst, 6, 6, 6, 6);
    cv::imwrite("copymakeborder.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

