#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	双线性插值
 * @param src		输入图像
 * @param dst		输出图像
 * @param sx		x方向缩放比例
 * @param sy		y方向缩放比例
 */
void linear(cv::Mat& src, cv::Mat& dst, float sx, float sy)
{
	dst = cv::Mat(round(sy*src.rows), round(sx*src.cols), src.type());
	for (int i = 0; i < dst.rows; i++)
	{
		float index_i = (i + 0.5) / sy - 0.5;
		if (index_i < 0) index_i = 0;
		if (index_i > src.rows - 1) index_i = src.rows - 1;
		int i1 = floor(index_i);
		int i2 = ceil(index_i);
		float u = index_i - i1;
		for (int j = 0; j < dst.cols; j++)
		{
			float index_j = (j + 0.5) / sx - 0.5;
			if (index_j < 0) index_j = 0;
			if (index_j > src.cols - 1) index_j = src.cols - 1;
			int j1 = floor(index_j);
			int j2 = ceil(index_j);
			float v = index_j - j1;
			dst.at<uchar>(i, j) = (1 - u)*(1 - v)*src.at<uchar>(i1, j1) + (1 - u)*v*src.at<uchar>(i1, j2) +
				u*(1 - v)*src.at<uchar>(i2, j1) + u*v*src.at<uchar>(i2, j2);
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;

	linear(src, dst, 2, 2);
	cv::imwrite("linear.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

