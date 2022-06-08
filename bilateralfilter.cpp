#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description: 		计算颜色空间
 * @param size			掩膜大小
 * @param sigmacolor	颜色空间滤波器标准差
 * @return 				颜色空间
 */
cv::Mat getcolor(int size, double sigmacolor)
{
    cv::Mat colormat = cv::Mat::zeros(256, 1, CV_64F);
	for (int i = 0; i < 256; ++i)
	{
		colormat.at<double>(i, 0) = exp((-i*i) / (2.0 * pow(sigmacolor, 2)));
	}
	return colormat;
}

/**
 * @description: 		计算坐标空间
 * @param size			掩膜大小
 * @param sigmasapce	坐标空间滤波器标准差
 * @return 				坐标空间
 */
cv::Mat getspace(int size, double sigmasapce)
{
    cv::Mat spacemat = cv::Mat::zeros(size, size, CV_64F);
	int center_i = (size - 1) / 2, center_j = (size - 1) / 2;
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			spacemat.at<double>(i, j) = exp((-pow(i - center_i, 2) - pow(j - center_j, 2)) / (2.0 * pow(sigmasapce, 2)));
		}
	}
	return spacemat;
}

/**
 * @description: 		双边滤波
 * @param src			输入图像
 * @param dst			输出图像
 * @param d				过滤过程中像素邻域直径
 * @param sigmacolor	颜色空间滤波器标准差
 * @param sigmaspace	坐标空间滤波器标准差
 */
void bilateralfilter(cv::Mat& src, cv::Mat& dst, int d, double sigmacolor, double sigmaspace)
{
	dst = cv::Mat::zeros(src.size(),src.type());
	cv::Mat mask = cv::Mat::zeros(d, d, CV_64F);
	cv::Mat colormat = getcolor(d, sigmacolor);
	cv::Mat spacemat = getspace(d, sigmaspace);
	cv::Mat mid;
	int b = (d - 1) / 2;
    cv::copyMakeBorder(src, mid, b, b, b, b, cv::BORDER_DEFAULT);

	for (int i = b; i < src.rows + b; ++i) 
	{
		for (int j = b; j < src.cols + b; ++j) 
		{
			int graydiff = 0;
			double sum =  0.0;

			for (int r = -b; r <= b; ++r) 
			{
				for (int c = -b; c <= b; ++c) 
				{
					graydiff = abs(mid.at<uchar>(i + r, j + c) - mid.at<uchar>(i, j));
					mask.at<double>(r + b, c + b) = colormat.at<double>(graydiff, 0) * spacemat.at<double>(r + b, c + b); 
					sum += mask.at<double>(r + b, c + b);				
				}
			}

			mask /=  sum; 
			double pixel = 0.0;

			for (int r = -b; r <= b; ++r) 
			{
				for (int c = -b; c <= b; ++c) 
				{
					pixel += mid.at<uchar>(i + r, j + c) * mask.at<double>(r + b, c + b); 
				}
			}

			dst.at<uchar>(i - b, j - b) = pixel;
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	bilateralfilter(src, dst, 13, 13 * 2, 13 / 2);
	cv::imwrite("bilateralfilter.jpg", dst);
	
	system("pause");
	return EXIT_SUCCESS;
}

