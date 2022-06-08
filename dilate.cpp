#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	膨胀运算
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void dilate(cv::Mat& src, cv::Mat& dst, cv::Size size)
{
    dst = cv::Mat::zeros(src.size(), src.type());
    cv::Mat mid;
	int h = (size.height - 1) / 2, w = (size.width - 1) / 2;
    cv::copyMakeBorder(src, mid, h, h, w, w, cv::BORDER_DEFAULT);
	for (int i = h; i < src.rows + h; ++i)
	{
		for (int j = w; j < src.cols + w; ++j)
		{
            cv::Mat kernel = mid(cv::Rect(j - w, i - h, size.width, size.height));
			double minValue, maxValue; 
            cv::Point minIdx, maxIdx;
            cv::minMaxLoc(kernel, &minValue, &maxValue, &minIdx, &maxIdx);
			dst.at<uchar>(i - h, j - w) = maxValue;
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;

	dilate(src, dst, cv::Size(13, 13));
    cv::imwrite("dilate.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

