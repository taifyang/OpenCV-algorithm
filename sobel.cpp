#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description: 	计算阶乘
 * @param n			自然数
 * @return			阶乘
 */
int factorial(int n) 
{
	int fac = 1;
	if (n == 0)	return fac;
	for (int i = 1; i <= n; ++i)	fac *= i;
	return fac;
}

/**
 * @description:    获得Sobel平滑算子
 * @param size		掩膜大小
 * @return			Sobel平滑算子
 */
cv::Mat getSobelSmooth(int size)
{
	int n = size - 1;
    cv::Mat SobelSmoothoper = cv::Mat::zeros(size, 1, CV_32F);
	for (int k = 0; k <= n; k++)
	{
		float *pt = SobelSmoothoper.ptr<float>(0);
		pt[k] = factorial(n) / (factorial(k)*factorial(n - k));
	}
	return SobelSmoothoper;
}

/**
 * @description:  	获得Sobel差分算子
 * @param size		掩膜大小
 * @return			Sobel差分算子
 */
cv::Mat getSobeldiff(int size)
{
    cv::Mat Sobeldiffoper = cv::Mat::zeros(cv::Size(size, 1), CV_32F);
    cv::Mat SobelSmooth = getSobelSmooth(size - 1);
	for (int k = 0; k < size; k++) 
	{
		if (k == 0)
			Sobeldiffoper.at<float>(0, k) = 1;
		else if (k == size - 1)
			Sobeldiffoper.at<float>(0, k) = -1;
		else
			Sobeldiffoper.at<float>(0, k) = SobelSmooth.at<float>(0, k) - SobelSmooth.at<float>(0, k - 1);
	}
	return Sobeldiffoper;
}

/**
 * @description:   	卷积实现
 * @param src		输入图像
 * @param dst		输出图像
 * @param kernel	卷积核
 */
void conv2D(cv::Mat& src, cv::Mat& dst, cv::Mat kernel)
{
    cv::flip(kernel, kernel, -1);
    cv::filter2D(src, dst, src.depth(), kernel);
}

/**
 * @description: 	可分离卷积———先水平方向卷积，后垂直方向卷积
 * @param src		输入图像
 * @param dst		输出图像
 * @param kernel_X	x方向卷积
 * @param kernel_Y	y方向卷积
 */
void sepConv2D_X_Y(cv::Mat& src, cv::Mat& dst, cv::Mat kernel_X, cv::Mat kernel_Y)
{
    cv::Mat dst_kernel_X;
    conv2D(src, dst_kernel_X, kernel_X); 
    conv2D(dst_kernel_X, dst, kernel_Y); 
}

/**
 * @description: 	可分离卷积———先垂直方向卷积，后水平方向卷积
 * @param src		输入图像
 * @param dst		输出图像
 * @param kernel_Y	y方向卷积
 * @param kernel_X	x方向卷积
 */
void sepConv2D_Y_X(cv::Mat& src, cv::Mat& dst, cv::Mat kernel_Y, cv::Mat kernel_X)
{
    cv::Mat dst_kernel_Y;
    conv2D(src, dst_kernel_Y, kernel_Y); 
    conv2D(dst_kernel_Y, dst, kernel_X); 
}

/**
 * @description: 	Sobel算子边缘检测
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void sobel(cv::Mat& src, cv::Mat& dst, int size)
{
	cv::Mat SobelSmoothoper = getSobelSmooth(size); 
	cv::Mat Sobeldiffoper = getSobeldiff(size);		
	cv::Mat dst_X, dst_Y;
	
	sepConv2D_X_Y(src, dst_Y, SobelSmoothoper, Sobeldiffoper.t()); 
	sepConv2D_Y_X(src, dst_X, SobelSmoothoper.t(), Sobeldiffoper); 

	dst = abs(dst_X) + abs(dst_Y);
	cv::convertScaleAbs(dst, dst);
	cv::convertScaleAbs(dst_X, dst_X);
	cv::convertScaleAbs(dst_Y, dst_Y);
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;

	sobel(src, dst, 3);
	cv::imwrite("sobel.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
