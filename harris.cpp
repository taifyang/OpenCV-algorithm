#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	harris角点提取
 * @param src		输入图像
 * @param dst		输出图像
 * @param ksize		sobel算子掩膜大小
 * @param k			系数
 */
void harris(cv::Mat& src, cv::Mat& dst, int ksize, int k)
{
	dst = src.clone();
	src.convertTo(src, CV_32F);

	cv::Mat img_x, img_y;
	cv::Sobel(src, img_x, src.depth(), 1, 0, ksize);
	cv::Sobel(src, img_y, src.depth(), 0, 1, ksize);

	cv::Mat img_xx, img_yy, img_xy;
	cv::GaussianBlur(img_x.mul(img_x), img_xx, cv::Size(9, 9), 0, 0);
	cv::GaussianBlur(img_y.mul(img_y), img_yy, cv::Size(9, 9), 0, 0);
	cv::GaussianBlur(img_x.mul(img_y), img_xy, cv::Size(9, 9), 0, 0);

    cv::Mat cim = cv::Mat::zeros(src.rows, src.cols, CV_32F);
	for (int i = 0; i < cim.rows; ++i)
	{
		for (int j = 0; j < cim.cols; ++j)
		{
			cim.at<float>(i, j) = (img_xx.at<float>(i, j) * img_yy.at<float>(i, j) - img_xy.at<float>(i, j) * img_xy.at<float>(i, j))
				- k * pow(img_xx.at<float>(i, j) + img_yy.at<float>(i, j), 2);
		}
	}

    cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

	int num = 0;
	for (int i = 1; i < cim.rows - 1; ++i)
	{
		for (int j = 1; j < cim.cols - 1; ++j)
		{
			float lmax = cim.at<float>(i, j);
			for (int x = -1; x <= 1; ++x)
			{
				for (int y = -1; y <= 1; ++y)
				{
					if (cim.at<float>(i + x, j + y) > lmax)
					{
						lmax = cim.at<float>(i + x, j + y);
					}
				}
			}

			float thresh = 1e8;
			if (cim.at<float>(i, j) == lmax && lmax > thresh)
			{	
				circle(dst, cv::Point(j, i), 1, cv::Scalar(0, 0, 255), -1);
				++num;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	cv::cvtColor(src, src, cv::COLOR_RGB2GRAY);

	harris(src, dst, 3, 0.06);
	cv::imwrite("harris.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

