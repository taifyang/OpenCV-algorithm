#include <iostream>
#include <opencv2/opencv.hpp>

// /**
//  * @description:	深度优先搜索
//  * @param src	输入图像
//  * @param dst	输出图像
//  * @param i		行号
//  * @param j		列号
//  * @param num	区域序号
//  */
// void dfs(cv::Mat& src, cv::Mat& dst, int i, int j, int num)
// {
// 	if (i >= 0 && i < src.rows && j >= 0 && j < src.cols && src.at<uchar>(i, j) && dst.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 0, 0))
// 	{
//      dst.at<Vec3b>(i, j) = cv::Vec3b(num * 80, 255 - num * 80, 0);
// 		dfs(src, dst, i - 1, j, num);
// 		dfs(src, dst, i + 1, j, num);
// 		dfs(src, dst, i, j - 1, num);
// 		dfs(src, dst, i, j + 1, num);
// 	}
// }

// void getconnecteddomain(cv::Mat& src, cv::Mat &dst)
// {
// 	dst = cv::Mat::zeros(src.size(), CV_8UC3);
// 	int num = 0;

// 	for (int i = 0; i < src.rows; ++i)
// 	{
// 		for (int j = 0; j < src.cols; ++j)
// 		{
// 			if (src.at<uchar>(i, j)!=0 && dst.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 0, 0))
// 			{
// 				dfs(src, dst, i, j, num);
// 				++num;
// 			}
// 		}
// 	}
// }

/**
 * @description:	连通域标记
 * @param src		输入图像
 * @param dst		输出图像
 */
void getconnecteddomain(cv::Mat& src, cv::Mat &dst)
{
    dst = cv::Mat::zeros(src.size(), CV_8UC3);
	int num = 0;

	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			int dir[4][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }};
            std::vector<cv::Point> pts;
            cv::Point pt,p;

            if (src.at<uchar>(i, j)!=0 && dst.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 0, 0))
			{
                pts.push_back(cv::Point(j, i));
                dst.at<cv::Vec3b>(i, j) = cv::Vec3b(num * 80, 255 - num * 80, 0);

				while (!pts.empty())
				{
					pt = pts.back();
					pts.pop_back();

					for (int k = 0; k < 4; ++k)
					{
						p.x = pt.x + dir[k][0];
						p.y = pt.y + dir[k][1];

						if (p.x < 0 || p.y < 0 || p.x >= src.cols || p.y >= src.rows)	continue;
                        if (src.at<uchar>(p.y,p.x)!=0 && dst.at<cv::Vec3b>(p.y, p.x) == cv::Vec3b(0, 0, 0))
						{							
                            dst.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(num * 80, 255 - num * 80, 0);
							pts.push_back(p);
						}
					}
				}

				++num;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	getconnecteddomain(src, dst);
	cv::imwrite("connectiondomain.jpg", dst);
	
	system("pause");
	return EXIT_SUCCESS;
}

