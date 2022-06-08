#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	拉普拉斯算子
 * @param src		输入图像
 * @param dst		输出图像
 */
void laplacian(cv::Mat& src, cv::Mat& dst)
{
	cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, 1, 0, 1, -4, 1, 0, 1, 0);
	cv::filter2D(src, dst, src.depth(), kernel);
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	laplacian(src, dst);
    cv::imwrite("laplacian.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
