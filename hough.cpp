#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	霍夫变换
 * @param edge		输入边缘
 * @param lines		检测直线
 * @param threshold	阈值
 */
void hough(cv::Mat& edge, std::vector<cv::Vec2f>& lines, int threshold)
{
    cv::Mat H = cv::Mat::zeros(2 * edge.rows + 2 * edge.cols, 180, CV_16S);
	float theta, rho;

	for (int i = 0; i < edge.rows; i++)
	{
		for (int j = 0; j < edge.cols; j++) 
		{
			if (edge.at<uchar>(i, j) > 0) 
			{
				for (theta = 0; theta < 180; ++theta) 
				{
					rho = round(i*sin(theta*CV_PI / 180) + j*cos(theta*CV_PI / 180));
					H.at<short>(rho + edge.rows + edge.cols, theta) += 1;
				}
			}
		}
	}

	for (int i = 0; i < H.rows; ++i)
	{
		for (int j = 0; j < H.cols; ++j)
		{
			if (H.at<short>(i, j) > threshold)
                lines.push_back(cv::Vec2f(i - edge.rows - edge.cols, j*CV_PI / 180));
		}
	}
}

int main(int argc, char** argv)
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;
    cv::Canny(src, dst, 50, 200, 3);

    std::vector<cv::Vec2f> lines;
	hough(dst, lines, 150);
    for (auto line : lines)	std::cout << line << std::endl;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
        cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = round(x0 + 1000 * (-b));
		pt1.y = round(y0 + 1000 * (a));
		pt2.x = round(x0 - 1000 * (-b));
		pt2.y = round(y0 - 1000 * (a));
		cv::line(src, pt1, pt2, cv::Scalar(55, 100, 195), 1, cv::LINE_AA);
	}
    cv::imwrite("hough.jpg", src);
	
	system("pause");
	return EXIT_SUCCESS;
}

