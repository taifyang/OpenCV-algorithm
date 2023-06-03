#include <iostream>
#include <cmath>
#include <numeric>
#include <opencv2/opencv.hpp>


void susan(cv::Mat& src, cv::Mat& dst, float t)
{
	cv::Mat susan_mask = cv::Mat::ones(7, 7, CV_8U);
	susan_mask.at<uchar>(0, 0) = 0; 
	susan_mask.at<uchar>(0, 1) = 0;
	susan_mask.at<uchar>(0, 5) = 0;
	susan_mask.at<uchar>(0, 6) = 0;
	susan_mask.at<uchar>(1, 0) = 0;
	susan_mask.at<uchar>(1, 6) = 0;
	susan_mask.at<uchar>(5, 0) = 0;
	susan_mask.at<uchar>(5, 6) = 0;
	susan_mask.at<uchar>(6, 0) = 0;
	susan_mask.at<uchar>(6, 1) = 0;
	susan_mask.at<uchar>(6, 5) = 0;
	susan_mask.at<uchar>(6, 6) = 0;

	//for (size_t i = 0; i < susan_mask.rows; i++)
	//{
	//	for (size_t j = 0; j < susan_mask.cols; j++)
	//	{
	//		std::cout << (int)susan_mask.at<uchar>(i, j) << " ";
	//	}
	//	std::cout << std::endl;
	//}
	
	int row_s = 3, col_s = 3;
	int row_e = src.rows - 3, col_e = src.cols - 3;
	float n_max = 0;
	cv::Mat	n_arr = 37 * cv::Mat::ones(src.size(), CV_8U);

	for (size_t i = row_s; i < row_e; i++)
	{
		for (size_t j = col_s; j < col_e; j++)
		{
			cv::Mat susan_zone = src(cv::Rect(j - 3, i - 3, 7, 7));
			//for (size_t i = 0; i < susan_zone.rows; i++)
			//{
			//	for (size_t j = 0; j < susan_zone.cols; j++)
			//	{
			//		std::cout << (int)susan_zone.at<uchar>(i, j) << " ";
			//	}
			//	std::cout << std::endl;
			//}

			std::vector<int> susan_zone_vec;
			for (size_t m = 0; m < susan_mask.rows; m++)
			{
				for (size_t n = 0; n < susan_mask.cols; n++)
				{
					if ((int)susan_mask.at<uchar>(m, n) != 0)
						susan_zone_vec.push_back((int)susan_zone.at<uchar>(m, n));
				}
			}

			//for (size_t k = 0; k < susan_zone_vec.size(); k++)
			//{
			//	std::cout << k << " " << susan_zone_vec[k] << std::endl;
			//}

			float r0 = src.at<uchar>(i, j);
			//std::cout << i << " " << j << " " << r0 << std::endl;

			std::vector<float> similarity(37);
			for (size_t k = 0; k < similarity.size(); k++)
			{
				similarity[k] = exp(-pow((1.0 * susan_zone_vec[k] - r0) / t, 6));
				//std::cout << similarity[k] << std::endl;
			}
			float n = std::accumulate(similarity.begin(), similarity.end(), 0.0f);
			//std::cout << i << " " << j << " " << n << std::endl;

			if (n > n_max)
				n_max = n;

			n_arr.at<uchar>(i, j) = n;
		}
	}

	float g = n_max / 2;
	//std::cout << g << std::endl;

	cv::Mat	R = cv::Mat::zeros(src.size(), CV_8U);
	dst = src.clone();
	cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
	for (size_t i = 0; i < R.rows; i++)
	{
		for (size_t j = 0; j < R.cols; j++)
		{
			if (n_arr.at<uchar>(i, j) < g)
			//std::cout << i << " " << j << " " << n_arr.at<float>(i, j) << " " << g << std::endl;
				R.at<uchar>(i, j) = g - n_arr.at<uchar>(i, j);
			if (R.at<uchar>(i, j) > 0)
				dst.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
		}
	}

#ifdef GRAVITY_FITER
	std::vector<std::vector<int>> x_label(7, std::vector<int>(7));
	std::vector<std::vector<int>> y_label(7, std::vector<int>(7));
	for (size_t i = 0; i < 7; i++)
	{
		for (size_t j = 0; j < 7; j++)
		{
			x_label[i][j] = j - 3;
			y_label[j][i] = j - 3;
		}
	}

	for (size_t i = row_s; i < row_e; i++)
	{
		for (size_t j = col_s; j < col_e; j++)
		{
			if (R.at<uchar>(i, j) == 0)
				continue;

			cv::Mat susan_zone = src(cv::Rect(j - 3, i - 3, 7, 7));

			float r0 = src.at<uchar>(i, j);

			std::vector<std::vector<float>> similarity(7, std::vector<float>(7));
			for (size_t m = 0; m < similarity.size(); m++)
			{
				for (size_t n = 0; n < similarity[0].size(); n++)
				{
					similarity[m][n] = exp(-pow((1.0 * (int)susan_zone.at<uchar>(m, n) - r0) / t, 6));
					//std::cout << similarity[m][n] << " ";
				}
				//std::cout << std::endl;
			}
			//std::cout << std::endl;

			float sum = 0.0f;
			float sum_x = 0.0f;
			float sum_y = 0.0f;
			for (size_t m = 0; m < similarity.size(); m++)
			{
				for (size_t n = 0; n < similarity[0].size(); n++)
				{
					if ((int)susan_mask.at<uchar>(m, n) == 1)
					{
						sum += similarity[m][n];
						sum_x += similarity[m][n] * x_label[m][n];
						sum_y += similarity[m][n] * y_label[m][n];
					}
				}
			}
			float g_x = sum_x / sum;
			float g_y = sum_y / sum;
			float distance = sqrt(g_x * g_x + g_y * g_y);

			float F = 1.5;
			if (distance < F)
				R.at<uchar>(i, j) = 0;
		}
	}

	dst = src.clone();
	cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
	for (size_t i = 0; i < R.rows; i++)
	{
		for (size_t j = 0; j < R.cols; j++)
		{
			if (R.at<uchar>(i, j))
				dst.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
		}
	}
#endif // GRAVITY_FITER
}


int main(int argc, char** argv)
{
	cv::Mat src = cv::imread("1.png", 0), dst;

	susan(src, dst, 10.0f);
	cv::imwrite("susan.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

