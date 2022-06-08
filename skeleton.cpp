#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @description:		骨架细化算法
 * @param src			输入图像
 * @param dst			输出图像
 * @param maxIterations	最大迭代次数
 */
void thinImage(cv::Mat& src, cv::Mat& dst, int maxIterations = -1)
{
	dst = src.clone();
    cv::Mat mid = src.clone();;
	int count = 0;  
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) 
			break;
        std::vector<uchar*> mFlag; 
		for (int i = 0; i < mid.rows; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < mid.cols; ++j)
			{
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == mid.cols - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == mid.cols - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == mid.rows- 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == mid.rows- 1 || j == mid.cols - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == mid.rows- 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{				
						mFlag.push_back(p + j); 
					}

				}
			}
		}

        for (std::vector<uchar*>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		if (mFlag.empty())
			break;
		else
			mFlag.clear();

		for (int i = 0; i < mid.rows; ++i)
		{
            uchar* p = dst.ptr<uchar>(i);
			for (int j = 0; j < mid.cols; ++j)
			{
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == mid.cols - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == mid.cols - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == mid.rows- 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == mid.rows- 1 || j == mid.cols - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == mid.rows- 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{					
						mFlag.push_back(p + j); 
					}

				}
			}
		}

        for (std::vector<uchar*>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		if (mFlag.empty())
			break;
		else
			mFlag.clear();
	}

	dst *= 255;
}

int main(int argc, char* argv[])
{
    cv::Mat img = cv::imread("1.jpg", 0), img1, img2 = img.clone();
    cv::cvtColor(img2, img2, cv::COLOR_GRAY2RGB);
    cv::threshold(img, img, 100, 1, cv::THRESH_BINARY);
	thinImage(img,img1);

    std::vector<cv::Point> P;
	for (int i = 0; i < img.cols; i++)
		for (int j = 0; j < img.rows; j++)
			if (img1.at<uchar>(j, i)>0)
                P.push_back(cv::Point(i, j));

	for (int i = 0; i < P.size(); i++)
        cv::circle(img2, cv::Point(P[i].x, P[i].y), 0.5, cv::Scalar(0, 0, 255), -1);
    cv::imwrite("skeleton.jpg", img2);

	system("pause");
	return EXIT_SUCCESS;
}

