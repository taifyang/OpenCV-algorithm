#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	计算阶乘
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
 * @description:   	获得Sobel差分算子
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
 * @description:	卷积实现
 * @param src		输入图像	
 * @param dst		输出图像
 * @param kernel	卷积核
 */
void conv2D(cv::Mat& src, cv::Mat& dst, cv::Mat kernel)
{
    cv::flip(kernel, kernel, -1);
    cv::filter2D(src, dst, CV_32F, kernel);
}

/**
 * @description:  	可分离卷积———先水平方向卷积，后垂直方向卷积
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
 * @description:  	可分离卷积———先垂直方向卷积，后水平方向卷积
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
 * @description:	Sobel算子边缘检测
 * @param src		输入图像
 * @param dst		输出图像
 * @param dst_X		x方向边缘
 * @param dst_Y		y方向边缘
 * @param size		掩膜大小
 */
void sobel(cv::Mat& src, cv::Mat& dst, cv::Mat& dst_X, cv::Mat& dst_Y, int size)
{
    cv::Mat SobelSmoothoper = getSobelSmooth(size);
    cv::Mat Sobeldiffoper = getSobeldiff(size);    

	sepConv2D_X_Y(src, dst_Y, SobelSmoothoper, Sobeldiffoper.t()); 
	sepConv2D_Y_X(src, dst_X, SobelSmoothoper.t(), Sobeldiffoper); 

	dst = abs(dst_X) + abs(dst_Y);
	convertScaleAbs(dst, dst);
}

/**
 * @description:  	确定一个点的坐标是否在图像内
 * @param r			点的行坐标
 * @param c			点的列坐标
 * @param rows		图像行数
 * @param cols		图像列数
 * @return			点的坐标是否在图像内
 */
bool checkInRange(int r, int c, int rows, int cols) 
{
	if (r >= 0 && r < rows && c >= 0 && c < cols)
		return true;
	else
		return false;
}

/**
 * @description:			从确定边缘点出发，延长边缘
 * @param edgeMag_noMaxsup	未经过极大值抑制的边缘强度
 * @param edge				图像边缘
 * @param Th				灰度阈值
 * @param r					点的行坐标
 * @param c					点的列坐标
 * @param rows				图像行数
 * @param cols				图像列数
 */
void trace(cv::Mat &edgeMag_noMaxsup, cv::Mat &edge, float Th, int r, int c, int rows, int cols)
{
	if (edge.at<uchar>(r, c) == 0)
	{
		for (int i = -1; i <= 1; ++i)
		{
			for (int j = -1; j <= 1; ++j)
			{
				if (checkInRange(r + i, c + j, rows, cols) && edgeMag_noMaxsup.at<float>(r + i, c + j) > Th)
					edge.at<uchar>(r, c) = 255;
			}
		}
	}
}

/**
 * @description:		Canny边缘检测
 * @param src			输入图像
 * @param dst			输出图像
 * @param Tl			低灰度阈值
 * @param Th			高灰度阈值
 * @param ksize			sobel算子掩膜大小
 * @param L2graydient	是否使用L2灰度梯度
 */
void canny(cv::Mat &src, cv::Mat &dst, float Tl, float Th, int ksize = 3, bool L2graydient = false)
{
    cv::GaussianBlur(src, src, cv::Size(3, 3), 0);
    cv::Mat dx, dy, sobel_dst;
	sobel(src, sobel_dst, dx, dy, ksize);

    cv::Mat edgeMag;
	if (L2graydient)
		magnitude(dx, dy, edgeMag);  
	else
		edgeMag = abs(dx) + abs(dy); 

    cv::Mat edgeMag_noMaxsup = cv::Mat::zeros(src.size(), CV_32F);
	for (int i = 1; i < src.rows - 1; ++i)
	{
		for (int j = 1; j < src.cols - 1; ++j) 
		{
			float angle =  atan2f(dy.at<float>(i, j), dx.at<float>(i, j)) / CV_PI * 180; 
			float cur = edgeMag.at<float>(i, j);  
			
			if (abs(angle) < 22.5 || abs(angle) > 157.5)
			{
				float left = edgeMag.at<float>(i, j - 1);
				float right = edgeMag.at<float>(i, j + 1);
				if (cur >= left && cur >= right)
					edgeMag_noMaxsup.at<float>(i, j) = cur;
			}

			if ((angle >= 67.5 && angle <= 112.5) || (angle >= -112.5 && angle <= -67.5)) 
			{
				float top = edgeMag.at<float>(i - 1, j);
				float down = edgeMag.at<float>(i + 1, j);
				if (cur >= top && cur >= down)
					edgeMag_noMaxsup.at<float>(i, j) = cur;
			}

			if ((angle>112.5 && angle <= 157.5) || (angle>-67.5 && angle <= -22.5)) 
			{
				float right_top = edgeMag.at<float>(i - 1, j + 1);
				float left_down = edgeMag.at<float>(i + 1, j - 1);
				if (cur >= right_top && cur >= left_down)
					edgeMag_noMaxsup.at<float>(i, j) = cur;
			}

			if ((angle >= 22.5 && angle < 67.5) || (angle >= -157.5 && angle < -112.5)) 
			{
				float left_top = edgeMag.at<float>(i - 1, j - 1);
				float right_down = edgeMag.at<float>(i + 1, j + 1);
				if (cur >= left_top && cur >= right_down)
					edgeMag_noMaxsup.at<float>(i, j) = cur;
			}

		}
	}

    dst = cv::Mat::zeros(src.size(), CV_8U);
	for (int i = 1; i < src.rows - 1; ++i) 
	{
		for (int j = 1; j < src.cols - 1; ++j) 
		{
			float mag = edgeMag_noMaxsup.at<float>(i, j);
			if (mag > Th)
				dst.at<uchar>(i, j) = 255;
			else if (mag < Tl)
				dst.at<uchar>(i, j) = 0;
			else
				trace(edgeMag_noMaxsup, dst, Th, i, j, src.rows, src.cols);
		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	canny(src, dst, 20, 60);
    cv::imwrite("canny.jpg", dst);
	
	system("pause");
	return EXIT_SUCCESS;
}

