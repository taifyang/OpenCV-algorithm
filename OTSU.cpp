#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	最大类间方差法图像分割
 * @param src		输入图像
 * @param dst		输出图像
 */
void OTSU(cv::Mat& src, cv::Mat& dst)
{
	int thresh = 0; 
	double var = 0.0; 
	uchar *data = src.data;
	int histogram[256] = { 0 }; 
	int totalnum = src.rows*src.cols; 

	for (int i = 0; i < src.rows; ++i)
	{
		const uchar* ptr = src.ptr<uchar>(i);
		for (int j = 0; j < src.cols; ++j)
		{        
			++histogram[ptr[j]]; 
		}
	}

	for (int i = 0; i< 256; i++)
	{
		double w0 = 0.0; 
		double w1 = 0.0; 
		double u0 = 0.0; 
		double u1 = 0.0; 

		for (int j = 0; j <= i; j++) 
		{
			w1 += histogram[j];  
			u1 += j*histogram[j]; 
		}

		u1 = (double) u1 / w1; 
		w1 = (double) w1 / totalnum; 

		for (int k = i + 1; k < 256; k++)
		{
			w0 += histogram[k]; 
			u0 += k*histogram[k]; 
		}

		u0 = (double) u0 / w0; 
		w0 = (double) w0 / totalnum; 

		double value = w0*w1*pow(u1 - u0, 2); 
		if (var < value)
		{
			var = value;
			thresh = i;
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

	OTSU(src, dst);
    cv::imwrite("otsu.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

