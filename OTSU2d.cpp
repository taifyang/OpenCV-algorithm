#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:	最大类间方差法（2d）图像分割
 * @param src		输入图像
 * @param dst		输出图像
 */
void OTSU2d(cv::Mat& src, cv::Mat& dst)
{
	double histogram[256][256] = { 0.0 }; 
	double trMatrix = 0.0;				  
	int totalnum = src.rows * src.cols;   

	for (int i = 0; i<src.rows; i++)
	{
		for (int j = 0; j<src.cols; j++)
		{
            uchar nData1 = src.at<uchar>(i, j);  
            uchar nData2 = 0;
			int nData3 = 0;         
			for (int m = i - 1; m <= i + 1; m++)
			{
				for (int n = j - 1; n <= j + 1; n++)
				{
					if ((m>=0) && (m<src.rows) && (n>=0) && (n<src.cols))
						nData3 += src.at<uchar>(m, n);  
				}
			}
            nData2 = (uchar)(nData3 / 9);    
			histogram[nData1][nData2]++;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			histogram[i][j] /= totalnum;  
		}
	}			

	double p_fi = 0.0;      
	double p_fj = 0.0;      
	double p_bi = 0.0;      
	double p_bj = 0.0;      
	double p_gi = 0.0;     
	double p_gj = 0.0;   
	double w_f = 0.0;     
	double w_b = 0.0;       
	double data1 = 0.0;
	double data2 = 0.0;
	double data3 = 0.0;
	double data4 = 0.0;   
	int threshold_s = 0;
	int threshold_t = 0;
	double temp = 0.0;     
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			p_gi += i*histogram[i][j];
			p_gj += j*histogram[i][j];
		}
	}
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			w_f += histogram[i][j];
			data1 += i*histogram[i][j];
			data2 += j*histogram[i][j];

			w_b = 1 - w_f;
			data3 = p_gi - data1;
			data4 = p_gj - data2;

			p_fi = data1 / w_f;
			p_fj = data2 / w_f;
			p_bi = data3 / w_b;
			p_bj = data4 / w_b;   
			trMatrix = ((w_f*p_gi - data1)*(w_f*p_gi - data1) + (w_f*p_gj - data1)*(w_f*p_gj - data2)) / (w_f*w_b);
			if (trMatrix > temp)
			{
				temp = trMatrix;
				threshold_s = i;
				threshold_t = j;
			}
		}
	}

	int thresh = (threshold_s + threshold_t) / 2;

	dst = src.clone();
	for (int i = 0; i < dst.rows; ++i)
	{
        uchar* ptr = dst.ptr<uchar>(i);
		for (int j = 0; j < dst.cols; ++j)
		{
			if (ptr[j] > thresh)
				ptr[j] = 255;
			else
				ptr[j] = 0;
		}
	}
}

int main(int argc, char* argv[])
{
    cv::Mat src = cv::imread("1.jpg", 0), dst;

	OTSU2d(src, dst);
    cv::imwrite("otsu2d.jpg", dst);

	system("pause");
	return EXIT_SUCCESS;
}
