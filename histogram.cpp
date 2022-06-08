#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	直方图双峰法图像分割
 * @param src		输入图像
 * @param dst		输出图像
 */
void histogram(cv::Mat& src, cv::Mat& dst)
{
	double minv, maxv;
    cv::Point pt_min, pt_max;
    cv::minMaxLoc(src, &minv, &maxv, &pt_min, &pt_max);
	double T = (minv + maxv) / 2;

	double thresh = 0; 
	double histogram[256] = { 0 }; 
	double totalnum = src.rows*src.cols; 

	for (int i = 0; i < src.rows; ++i)
	{
		const uchar* ptr = src.ptr<uchar>(i);
		for (int j = 0; j < src.cols; ++j)
		{
			++histogram[ptr[j]]; 
		}
	}

	while (abs(thresh - T) > 1)
	{
		T = thresh;
		double w0 = 0.0; 
		double w1 = 0.0; 
		double u0 = 0.0; 
		double u1 = 0.0; 

		for (int j = 0; j <= T; j++) 
		{
			w1 += histogram[j]; 
			u1 += j*histogram[j]; 
		}

		u1 = u1 / w1; 

		for (int k = T + 1; k < 256; k++)
		{
			w0 += histogram[k]; 
			u0 += k*histogram[k]; 
		}

		u0 = u0 / w0; 
		thresh = (u0 + u1) / 2;
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

	histogram(src, dst);
    cv::imwrite("histogram.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
