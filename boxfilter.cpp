#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description: 	计算积分图
 * @param img		输入图像
 * @param sum		积分图
 */
void sumRegion(cv::Mat& img, cv::Mat& sum)
{
	for (size_t i = 1; i < sum.rows; i++)
	{
		for (size_t j = 1; j < sum.cols; j++)
		{
			sum.at<double>(i, j) = sum.at<double>(i - 1, j) + sum.at<double>(i, j - 1) - sum.at<double>(i - 1, j - 1) + img.at<uchar>(i - 1, j - 1);
		}
	}	
}

/**
 * @description: 	计算区域和
 * @param sum		积分图
 * @param row1
 * @param col1
 * @param row2
 * @param col2
 * @return			区域和
 */
int calcRegion(cv::Mat& sum, int row1, int col1, int row2, int col2)
{
	return sum.at<double>(row2 + 1, col2 + 1) - sum.at<double>(row1, col2 + 1) - sum.at<double>(row2 + 1, col1) + sum.at<double>(row1, col1);
}

/**
 * @description: 	方框滤波
 * @param src		输入图像
 * @param dst		输出图像
 * @param size		掩膜大小
 */
void boxfilter(cv::Mat& src, cv::Mat& dst, cv::Size size)
{	
    dst = cv::Mat::zeros(src.rows, src.cols, src.type());
	int w = (size.width - 1) / 2, h = (size.height - 1) / 2;
    cv::Mat mid;
    cv::copyMakeBorder(src, mid, h, h, w, w, cv::BORDER_DEFAULT);
    cv::Mat sum = cv::Mat::zeros(mid.rows + 1, mid.cols + 1, CV_64F);
	sumRegion(mid, sum);

	for (size_t i = h ; i < src.rows + h ; i++)
	{
		for (size_t j = w ; j < src.cols + w ; j++)
		{			
			dst.at<uchar>(i - h , j - w) = calcRegion(sum, i - h, j - w, i + h, j + w) / (size.width*size.height);
		}
	}
}

///**
// * @description: 	方框滤波
// * @param src		输入图像
// * @param dst		输出图像
// * @param size		掩膜大小
// */
//void boxfilter(cv::Mat& src, cv::Mat& dst, cv::Size size)
//{
//    cv::Mat kernel = cv::Mat::zeros(size, CV_64F);
//	for (int i = 0; i < size.height; ++i)
//	{
//		for (int j = 0; j < size.width; ++j)
//		{
//			kernel.at<double>(i, j) = 1.0 / (size.height*size.width);
//		}
//	}
//    cv::filter2D(src, dst, src.depth(), kernel);
//}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	boxfilter(src, dst, cv::Size(3, 3));
	cv::imwrite("boxfilter.jpg", dst);
	
	system("pause");
	return EXIT_SUCCESS;
}

