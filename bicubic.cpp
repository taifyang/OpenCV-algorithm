#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description: 	计算权重
 * @param c			c就是u和v，横坐标和纵坐标的输出计算方式一样
 * @param a			系数
 * @return 			权重
 */
std::vector<float> getWeight(float c, float a = -1.0)
{
	std::vector<float> temp = { 1 + c, c, 1 - c, 2 - c };

	std::vector<float> weight(4);
	weight[0] = a * pow(fabs(temp[0]), 3) - 5 * a * pow(fabs(temp[0]), 2) + 8 * a * fabs(temp[0]) - 4 * a;
	weight[1] = (a + 2) * pow(fabs(temp[1]), 3) - (a + 3) * pow(fabs(temp[1]), 2) + 1;
	weight[2] = (a + 2) * pow(fabs(temp[2]), 3) - (a + 3) * pow(fabs(temp[2]), 2) + 1;
	weight[3] = a * pow(fabs(temp[3]), 3) - 5 * a * pow(fabs(temp[3]), 2) + 8 * a * fabs(temp[3]) - 4 * a;

	return weight;
}

/**
 * @description: 	双三次插值
 * @param src		输入图像
 * @param dst		输出图像
 * @param sx		x方向缩放系数
 * @param sy		y方向缩放系数
 */
void bicubic(cv::Mat& src, cv::Mat& dst, float sx, float sy)
{
	int dst_cols = sx*src.cols, dst_rows = sy*src.rows;
	dst.create(dst_rows, dst_cols, src.type());

	for (int i = 0; i < dst_rows; ++i)
	{
		float index_i = (i + 0.5) / sy - 0.5; 
		if (index_i < 0) index_i = 0;
		if (index_i > src.rows - 1) index_i = src.rows - 1;

		int i0 = index_i - 1, i1 = index_i, i2 = index_i + 1, i3 = index_i + 2;
		float u = index_i - i1;
		std::vector<float> weight_u = getWeight(u);

		for (int j = 0; j < dst_cols; ++j)
		{
			float index_j = (j + 0.5) / sx - 0.5;
			if (index_j < 0) index_j = 0;
			if (index_j > src.cols - 1) index_j = src.cols - 1;

			int j0 = index_j - 1, j1 = index_j, j2 = index_j + 1, j3 = index_j + 2;
			float v = index_j - j1;
			std::vector<float> weight_v = getWeight(v);

			float temp = 0;
			for (int s = 0; s <= 3; s++)
			{
				for (int t = 0; t <= 3; t++)
				{
					temp += src.at<uchar>(i1 + s - 1, j1 + t - 1)*weight_u[s] * weight_v[t];
				}
			}

			if (temp < 0)	temp = 0;
			if (temp > 255)	temp = 255;
			dst.at<uchar>(i, j) = temp;
		}
	}
}

int main(int argc, char* argv[])
{
	cv::Mat src = cv::imread("1.jpg", 0), dst;
	
	bicubic(src, dst, 2, 2);
	cv::imwrite("bicubic.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}

