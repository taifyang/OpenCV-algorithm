#include <iostream>
#include <opencv2/opencv.hpp>


void shi_tomas(cv::Mat& src, cv::Mat& dst, int maxCorners, float qualityLevel, int minDistance)
{
	int block_size = 3;
	int ksize = 3; 
	float k = 0.04;

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
			cim.at<float>(i, j) = 0.5 * (sum_xx + sum_yy) - 0.5 * sqrt(pow(sum_xx - sum_yy, 2) + 4 * pow(sum_xy, 2));
		}
	}

	double minv, maxv;
	cv::Point pt_min, pt_max;
	cv::minMaxLoc(cim, &minv, &maxv, &pt_min, &pt_max);
 
	for (int i = 0; i < cim.rows; ++i)
	{
		for (int j = 0; j < cim.cols; ++j)
		{
			if (cim.at<float>(i, j) < maxv * qualityLevel)
			{
				cim.at<float>(i, j) = 0;
			}
		}
	}

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)), dilated;
	cv::dilate(cim, dilated, element);

	for (int i = 0; i < cim.rows; ++i)
	{
		for (int j = 0; j < cim.cols; ++j)
		{
			if (cim.at<float>(i, j) != dilated.at<float>(i, j))
			{
				cim.at<float>(i, j) = 0;
			}
		}
	}

	int num = 0;
	cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
	while (num < maxCorners)
	{
		cv::minMaxLoc(cim, &minv, &maxv, &pt_min, &pt_max);
		if (maxv == 0)
			break;

		dst.at<cv::Vec3b>(pt_max.y, pt_max.x) = cv::Vec3b(0, 0, 255);
		num++;

		for (int y = pt_max.y - minDistance; y <= pt_max.y + minDistance; ++y)
		{
			if (y < 0 || y >= cim.rows)
				continue;
			for (int x = pt_max.x - minDistance; x <= pt_max.x + minDistance; ++x)
			{
				if (x < 0 || x >= cim.cols)
					continue;
				if (sqrt(pow(y - pt_max.y, 2) + pow(x - pt_max.x, 2)) <= minDistance)
				{
					cim.at<float>(y, x) = 0;
				}
			}
		}
	} 
	
	std::cout << num << std::endl;
}


int main(int argc, char** argv)
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;

	shi_tomas(src, dst, 1000, 0.01, 7);
	cv::imwrite("my-shi-tomas.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

