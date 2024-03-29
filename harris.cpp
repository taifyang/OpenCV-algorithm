#include <iostream>
#include <opencv2/opencv.hpp>


void harris(cv::Mat& src, cv::Mat& dst, int block_size, int ksize, float k)
{
	dst = src.clone();
	src.convertTo(src, CV_32F);

	double scale = 1.0 / ((ksize - 1) * 2 * block_size * 255);

	cv::Mat img_x, img_y;
	cv::Sobel(src, img_x, -1, 1, 0, ksize, scale);
	cv::Sobel(src, img_y, -1, 0, 1, ksize, scale);

	cv::Mat img_xx, img_yy, img_xy;
	cv::boxFilter(img_x.mul(img_x), img_xx, -1, cv::Size(block_size, block_size));
	cv::boxFilter(img_y.mul(img_y), img_yy, -1, cv::Size(block_size, block_size));
	cv::boxFilter(img_x.mul(img_y), img_xy, -1, cv::Size(block_size, block_size));

	int radius = int((block_size - 1) / 2);
	int N_pre = radius;
	int N_post = block_size - radius - 1;
	int row_s = N_pre;
	int col_s = N_pre;
	int row_e = src.rows - N_post;
	int col_e = src.cols - N_post;

	cv::Mat cim = cv::Mat::zeros(src.rows, src.cols, CV_32F);
	for (int i = row_s; i < row_e; ++i)
	{
		for (int j = col_s; j < col_e; ++j)
		{
			float sum_xx = img_xx.at<float>(i, j);
			float sum_yy = img_yy.at<float>(i, j);
			float sum_xy = img_xy.at<float>(i, j);

			float det = sum_xx * sum_yy - sum_xy * sum_xy;
			float trace = sum_xx + sum_yy;
			float res = det - k * trace * trace;
			cim.at<float>(i, j) = res;
		}
	}

	double minv, maxv;
	cv::Point pt_min, pt_max;
	cv::minMaxLoc(cim, &minv, &maxv, &pt_min, &pt_max);
	std::cout << maxv << std::endl;

	cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

	int num = 0;
	for (int i = 0; i < cim.rows; ++i)
	{
		for (int j = 0; j < cim.cols; ++j)
		{
			if (cim.at<float>(i, j) > 0.01* maxv)
			{
				dst.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
				num++;
			}
		}
	}
	std::cout << num << std::endl;
}


int main(int argc, char** argv)
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	cv::Mat srccopy = src.clone();

	harris(src, dst, 2, 3, 0.04);
	cv::imwrite("myharris.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

