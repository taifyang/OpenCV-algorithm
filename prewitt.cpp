#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	prewitt算子
 * @param src		输入图像
 * @param dst		输出图像
 */
void prewitt(cv::Mat& src, cv::Mat& dst)
{
	cv::Mat getPrewitt_horizontal = (cv::Mat_<float>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
	cv::Mat getPrewitt_vertical = (cv::Mat_<float>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1); 
	cv::Mat getPrewitt_diagonal1 = (cv::Mat_<float>(3, 3) << -1, -1, 0, -1, 0, 1, 0, 1, 1); 
	cv::Mat getPrewitt_diagonal2 = (cv::Mat_<float>(3, 3) << 0, 1, 1, -1, 0, 1, -1, -1, 0); 

	cv::flip(getPrewitt_horizontal, getPrewitt_horizontal, -1);
	cv::flip(getPrewitt_vertical, getPrewitt_vertical, -1);
	cv::flip(getPrewitt_diagonal1, getPrewitt_diagonal1, -1);
	cv::flip(getPrewitt_diagonal2, getPrewitt_diagonal2, -1);

	cv::Mat dst1, dst2, dst3, dst4;
	cv::filter2D(src, dst1, src.depth(), getPrewitt_horizontal); 
	cv::filter2D(src, dst2, src.depth(), getPrewitt_vertical); 
	cv::filter2D(src, dst3, src.depth(), getPrewitt_diagonal1); 
	cv::filter2D(src, dst4, src.depth(), getPrewitt_diagonal2); 

	cv::convertScaleAbs(dst1, dst1);
	cv::convertScaleAbs(dst2, dst2);
	cv::convertScaleAbs(dst3, dst3);
	cv::convertScaleAbs(dst4, dst4);
	dst = dst1 + dst2;
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	prewitt(src, dst);
    cv::imwrite("prewitt.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
