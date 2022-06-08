#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	计算掩膜的中值
 * @param kernel	掩膜
 * @return			掩膜的中值
 */
int getmedian(cv::Mat& kernel)
{
    std::vector<uchar> v;
	for (int i = 0; i < kernel.rows; ++i)
	{
		for (int j = 0; j < kernel.cols; ++j)
		{
			v.push_back(kernel.at<uchar>(i, j));
		}
	}
	sort(v.begin(), v.end());
	return v[v.size() / 2];
}

/**
 * @description:	中值滤波
 * @param src		输入图像
 * @param dst		输出图像
 * @param ksize		掩膜大小
 */
void medianfilter(cv::Mat& src, cv::Mat& dst, int ksize)
{
    dst = cv::Mat::zeros(src.size(), src.type());
    cv::Mat mid;
	int k = (ksize - 1) / 2;
    cv::copyMakeBorder(src, mid, k, k, k, k, cv::BORDER_REPLICATE);
	for (int i = k; i < src.rows + k; ++i)
	{
		for (int j = k; j < src.cols + k; ++j)
		{
            cv::Mat kernel = mid(cv::Rect(j - k, i - k, ksize, ksize));
			dst.at<uchar>(i - k, j - k) = getmedian(kernel);
		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	medianfilter(src, dst, 3);
	cv::imwrite("medianfilter.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
