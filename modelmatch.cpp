#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	模板匹配
 * @param src		输入图像
 * @param temp		模板图像
 * @param dst		输出图像
 */
void matchtemplate(cv::Mat& src, cv::Mat& temp, cv::Mat& dst)
{
	for (int i = 0; i < dst.cols; ++i)
	{
		for (int j = 0; j < dst.rows; ++j)
		{
			int R = 0;
			for (int m = 0; m < temp.cols; ++m)
			{
				for (int n = 0; n < temp.rows; ++n)
				{
					R += pow(src.at<uchar>(j + n, i + m) - temp.at<uchar>(n, m), 2);
				}
			}
			dst.at<int>(j, i) = R;
		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), temp = cv::imread("1part.jpg", 0), dst;
    dst.create(cv::Size(src.cols - temp.cols + 1, src.rows - temp.rows + 1), CV_32SC1);

	matchtemplate(src, temp, dst);
	double minValue, maxValue;
	cv::Point minLocation, maxLocation;
	cv::minMaxLoc(dst, &minValue, &maxValue, &minLocation, &maxLocation);
	std::cout << minLocation << std::endl;

	system("pause");
	return EXIT_SUCCESS;
}

