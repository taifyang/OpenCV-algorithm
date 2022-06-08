#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	直线拟合
 * @param points	输入点集
 * @param line_para	直线参数
 */
void fitline(std::vector<cv::Point>& points, cv::Vec4f& line_para)
{
	int N = points.size();
	if (N < 2)	return; 

	double sum_x = 0, sum_y = 0, sum_xx = 0, sum_xy = 0, sum_yy = 0;
	for (int i = 0; i < N; i++)
	{
		sum_x += points[i].x;
		sum_y += points[i].y;
		sum_xx += points[i].x * points[i].x;
		sum_xy += points[i].x * points[i].y;
		sum_yy += points[i].y * points[i].y;
	}

	float k = (N*sum_xy-sum_x*sum_y) / (N*sum_xx - sum_x*sum_x);
	float b = (sum_xx*sum_y - sum_x*sum_xy) / (N*sum_xx - sum_x*sum_x);
	std::cout << "k = " << k << ", b = " << b << std::endl;

	float A, B, C;
	float mean_x = sum_x / N, mean_y = sum_y / N, mean_xx = sum_xx / N, mean_xy = sum_xy / N, mean_yy = sum_yy / N;
	cv::Mat m = (cv::Mat_<float>(2, 2) << mean_xx - mean_x*mean_x, mean_xy - mean_x*mean_y, mean_xy - mean_x*mean_y, mean_yy - mean_y*mean_y);

	cv::Mat eigenvalue, eigenvector;
	cv::eigen(m * N, eigenvalue, eigenvector);

	float v0 = eigenvalue.at<float>(0, 0), v1 = eigenvalue.at<float>(0, 1);
	if (abs(v0) < abs(v1))
	{
		A = eigenvector.at<float>(0, 0);
		B = eigenvector.at<float>(0, 1);	
	}
	else {
		A = eigenvector.at<float>(1, 0);
		B = eigenvector.at<float>(1, 1);
	}
	C = -(A*mean_x + B*mean_y);
	std::cout << "k = " << -A / B << ", b = " << -C / B << std::endl;
	
	line_para[0] = B;
	line_para[1] = -A;
	line_para[2] = mean_x;
	line_para[3] = mean_y;
}

int main(int argc, char* argv[])
{
	std::vector<cv::Point> points;
	points.push_back(cv::Point(48, 58));
	points.push_back(cv::Point(105, 98));
	points.push_back(cv::Point(155, 160));
	points.push_back(cv::Point(212, 220));
	points.push_back(cv::Point(248, 260));
	points.push_back(cv::Point(320, 300));
	points.push_back(cv::Point(350, 360));
	points.push_back(cv::Point(412, 400));

	cv::Vec4f line_para;
	fitline(points, line_para);
	std::cout << "line_para = " << line_para << std::endl;

	system("pause");
	return EXIT_SUCCESS;
}

