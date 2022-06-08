#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	计算外接矩形
 * @param img		输入图像
 * @return			外接矩形
 */
cv::Rect boundingrect(cv::Mat& img)
{
    cv::Rect rect(0, 0, 0, 0);

	bool flag = false;
	for (size_t i = 0; i < img.cols; ++i)
	{
		for (size_t j = 0; j < img.rows; ++j)
		{
			if (img.at<uchar>(j, i))
			{
				rect.x = i;
				flag = true;
				break;
			}
		}
		if (flag)	break;
	}

	flag = false;
	for (size_t i = img.cols - 1; i > 0; --i)
	{
		for (size_t j = 0; j < img.rows; ++j)
		{
			if (img.at<uchar>(j, i))
			{
				rect.width = i - rect.x;
				flag = true;
				break;
			}
		}
		if (flag)	break;
	}

	flag = false;
	for (size_t i = 0; i < img.rows; ++i)
	{
		for (size_t j = 0; j < img.cols; ++j)
		{
			if (img.at<uchar>(i, j))
			{
				rect.y = i;
				flag = true;
				break;
			}
		}
		if (flag)	break;
	}

	flag = false;
	for (size_t i = img.rows - 1; i >= 0; --i)
	{
		for (size_t j = 0; j < img.cols; ++j)
		{
			if (img.at<uchar>(i, j))
			{
				rect.height = i - rect.y;
				flag = true;
				break;
			}
		}
		if (flag)	break;
	}
	return rect;
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0),  dst;
	
    cv::Rect rect = boundingrect(src);
    std::cout << rect << std::endl;
	
	system("pause");
    return EXIT_SUCCESS;
}

