#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	最大熵值法图像分割
 * @param src		输入图像
 * @param dst		输出图像
 */
void maxentropy(cv::Mat& src, cv::Mat& dst)
{
	int thresh = 0;
	int histogram[256] = { 0 };
	for (int i = 0; i < src.rows; ++i)
	{
		const uchar* ptr = src.ptr<uchar>(i);
		for (int j = 0; j < src.cols; ++j)
		{
			if (ptr[j] == 0) continue;
			histogram[ptr[j]]++;
		}
	}

	float p = 0.0; 
	float max_entropy = 0.0; 
	int totalnum = src.rows*src.cols;
	for (int i = 0; i < 256; ++i)
	{
		float front_entropy = 0.0; 
		float back_entropy = 0.0; 

		int frontnum = 0;
		for (int j = 0; j < i; ++j) 
		{
			frontnum += histogram[j];
		}
		for (int j = 0; j < i; ++j)
		{
			if (histogram[j] != 0)
			{
				p = (float)histogram[j] / frontnum;
				front_entropy += p*log(1 / p);
			}
		}

		for (int k = i; k < 256; ++k) 
		{
			if (histogram[k] != 0)
			{
				p = (float)histogram[k] / (totalnum - frontnum);
				back_entropy += p*log(1 / p);
			}
		}

		if (front_entropy + back_entropy > max_entropy) 
		{
			max_entropy = front_entropy + back_entropy;
			thresh = i ;
		}
	}

	dst = src.clone();
	for (int i = 0; i < dst.rows; ++i)
	{
		uchar* ptr = dst.ptr<uchar>(i);
		for (int j = 0; j < dst.cols; ++j)
		{
			if (ptr[j] > thresh)
				ptr[j] = 255;
			else
				ptr[j] = 0;
		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	maxentropy(src, dst);
    cv::imwrite("maxentropy.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
