#include <iostream>
#include <opencv2/opencv.hpp>


void fast(cv::Mat& src, cv::Mat& dst, int t = 10)
{
	int fast_N = 9;
	int detect_radius = 3;
	std::vector<int> row_mask = { 0,0,1,2,3,4,5,6,6,6,5,4,3,2,1,0 };
	std::vector<int> col_mask = { 3,4,5,6,6,6,5,4,3,2,1,0,0,0,1,2 };

	cv::Mat score_arr = cv::Mat::zeros(src.size(), CV_32F);
	dst = src.clone();
	cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
	int row_s = detect_radius, col_s = detect_radius;
	int row_e = src.rows - detect_radius, col_e = src.cols - detect_radius;

	for (size_t i = row_s; i < row_e; i++)
	{
		for (size_t j = col_s; j < col_e; j++)
		{
			cv::Mat fast_zone = src(cv::Rect(j - detect_radius, i - detect_radius, 2 * detect_radius + 1, 2 * detect_radius + 1));
			//for (size_t i = 0; i < fast_zone.rows; i++)
			//{
			//	for (size_t j = 0; j < fast_zone.cols; j++)
			//	{
			//		std::cout << (int)fast_zone.at<uchar>(i, j) << " ";
			//	}
			//	std::cout << std::endl;
			//}

			int r0 = (int)src.at<uchar>(i, j);
			std::vector<int> condition1(row_mask.size()), condition2(row_mask.size());
			float sum1=0.0, sum2=0.0;
			for (size_t k = 0; k < row_mask.size(); k++)
			{
				if ((int)fast_zone.at<uchar>(row_mask[k], col_mask[k]) - r0 > t)
				{
					condition1[k] = 1;
					sum1 += (int)fast_zone.at<uchar>(row_mask[k], col_mask[k]) - r0;
				}
				if ((int)fast_zone.at<uchar>(row_mask[k], col_mask[k]) - r0 < -t)
				{
					condition2[k] = 1;
					sum2 += r0 - (int)fast_zone.at<uchar>(row_mask[k], col_mask[k]);
				}
			}

			if (condition1[0] + condition1[8] >= 1 && condition1[4] + condition1[12] >= 1)
			{
				std::vector<int> temp = condition1;
				temp.insert(temp.end(), condition1.begin(), condition1.end() - 1);
				int len = 0, max_len = 0;
				for (size_t k = 0; k < temp.size(); k++)
				{
					if (temp[k])
						len += 1;
					else
						len = 0;
					max_len = std::max(max_len, len);
				}
				if (max_len >= fast_N)
				{
					dst.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
					score_arr.at<float>(i, j) = sum1;
				}
			}
			if (condition2[0] + condition2[8] >= 1 && condition2[4] + condition2[12] >= 1)
			{
				std::vector<int> temp = condition2;
				temp.insert(temp.end(), condition2.begin(), condition2.end() - 1);
				int len = 0, max_len = 0;
				for (size_t k = 0; k < temp.size(); k++)
				{
					if (temp[k])
						len += 1;
					else
						len = 0;
					max_len = std::max(max_len, len);
				}
				if (max_len >= fast_N)
				{
					dst.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
					score_arr.at<float>(i, j) = sum2;
				}
			}

		}
	}


#ifdef NMS
	for (size_t i = row_s; i < row_e; i++)
	{
		for (size_t j = col_s; j < col_e; j++)
		{
			if (dst.at<cv::Vec3b>(i, j) != cv::Vec3b(0, 0, 255))
				continue;
			cv::Mat zone = score_arr(cv::Rect(j - detect_radius / 2, i - detect_radius / 2, detect_radius + 1, detect_radius + 1));
			int num = 0;
			for (size_t m = 0; m < zone.rows; m++)
			{
				for (size_t n = 0; n < zone.cols; n++)
				{
					if (zone.at<float>(m, n) > score_arr.at<float>(i, j))
						num++;
				}
			}
			if (num > 0)
			{
				dst.at<cv::Vec3b>(i, j)[0] = src.at<uchar>(i, j);
				dst.at<cv::Vec3b>(i, j)[1] = src.at<uchar>(i, j);
				dst.at<cv::Vec3b>(i, j)[2] = src.at<uchar>(i, j);
			}

		}
	}
#endif // NMS

}


int main(int argc, char** argv)
{
	cv::Mat src = cv::imread("1.png", 0), dst;

	fast(src, dst, 10);
	cv::imwrite("myfast.png", dst);

	system("pause");
	return EXIT_SUCCESS;
}