#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	腐蚀运算
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void erode(cv::Mat& src, cv::Mat& dst, cv::Size size)
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
			dst.at<uchar>(i - h, j - w) = minValue;
		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	erode(src, dst, cv::Size(3, 3));
    cv::imwrite("erode.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

