#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	最近邻插值
 * @param src		输入图像
 * @param dst		输出图像
 * @param sx		x方向缩放比例
 * @param sy		y方向缩放比例
 */
void nearest(cv::Mat& src, cv::Mat& dst, float sx, float sy)
{
	dst = cv::Mat(round(src.rows * sy), round(src.cols * sx), src.type());
	for (int i = 0; i < dst.rows; i++)
	{
		float index_i = (i + 0.5) / sy - 0.5;
		if (index_i < 0) index_i = 0;
		if (index_i > src.rows - 1) index_i = src.rows - 1;
		int i_index = round(index_i);
		for (int j = 0; j < dst.cols; j++)
		{
			float index_j = (j + 0.5) / sx - 0.5;
			if (index_j < 0) index_j = 0;
			if (index_j > src.cols - 1) index_j = src.cols - 1;
			int j_index = round(index_j);
			dst.at<uchar>(i, j) = src.at<uchar>(i_index, j_index);
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;

	nearest(src, dst, 2, 2);
	cv::imwrite("nearest.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

