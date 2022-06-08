#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	直方图均衡化
 * @param src		输入图像
 * @param dst		输出图像
 */
void equalizehist(cv::Mat &src, cv::Mat &dst)
{
	dst.create(src.size(), src.type());

	int histogram[256] = { 0 };
	int totalnum = src.rows*src.cols;
	uchar *data = src.data;
	for (int i = 0; i < totalnum; ++i)
	{
		++histogram[data[i]];
	}

	int LUT[256], sum = 0;
	for (int i = 0; i < 256; ++i)
	{
		sum += histogram[i];
		LUT[i] = 255.0*sum / totalnum;
	}

	uchar *dataOfSrc = src.data;
	uchar *dataOfDst = dst.data;
	for (int i = 0; i < totalnum; ++i)
		dataOfDst[i] = LUT[dataOfSrc[i]];
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	equalizehist(src, dst);
    cv::imwrite("equalizehist.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

