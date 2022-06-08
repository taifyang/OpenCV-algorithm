#include <iostream>
#include <opencv2/opencv.hpp>

enum adaptiveMethod
{
	meanFilter,
	gaaussianFilter,
	medianFilter
};

/**
 * @description:	����Ӧ��ֵ��
 * @param src     	����ͼ��
 * @param dst     	���ͼ��
 * @param size   	�˲��ߴ�
 * @param c         ƽ����
 * @param method    ����Ӧ����
 */
void adaptivethreshold(cv::Mat &src, cv::Mat &dst, int size, double c, adaptiveMethod method = meanFilter)
{
	cv::Mat smooth;
	switch (method)
	{
	case meanFilter:
		cv::blur(src, smooth, cv::Size(size, size)); 
		break;
	case gaaussianFilter:
		cv::GaussianBlur(src, smooth, cv::Size(size, size), 0, 0); 
		break;
	case medianFilter:
		cv::medianBlur(src, smooth, size); 
		break;
	default:
		break;
	}

	smooth = smooth - c;

	dst = src.clone();
	for (int i = 0; i < src.rows; ++i)
	{
		const uchar *srcptr = src.ptr<uchar>(i);
		const uchar *smoothptr = smooth.ptr<uchar>(i);
		uchar *dstptr = dst.ptr<uchar>(i);
		for (int j = 0; j < src.cols; ++j)
		{
			if (srcptr[j] > smoothptr[j])
				dstptr[j] = 255;
			else
				dstptr[j] = 0;
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	adaptivethreshold(src, dst, 13, 10);
	cv::imwrite("adaptivethreshold.jpg", dst);
	
	system("pause");
	return EXIT_SUCCESS;
}
