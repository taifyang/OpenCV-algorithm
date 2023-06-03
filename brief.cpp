#include<iostream>
#include<opencv2/opencv.hpp>

int PATCH_SIZE = 48;
int KERNEL_SIZE = 9;


int smoothedSum(const cv::Mat& sum, const cv::KeyPoint& pt, int y, int x, bool use_orientation, cv::Matx21f R)
{
	const int HALF_KERNEL = KERNEL_SIZE / 2;

	if (use_orientation)
	{
		int rx = (int)(((float)x) * R(1, 0) - ((float)y) * R(0, 0));
		int ry = (int)(((float)x) * R(0, 0) + ((float)y) * R(1, 0));
		if (rx > 24) rx = 24;
		if (rx < -24) rx = -24;
		if (ry > 24) ry = 24;
		if (ry < -24) ry = -24;
		x = rx; y = ry;
	}
	const int img_y = (int)(pt.pt.y + 0.5) + y;
	const int img_x = (int)(pt.pt.x + 0.5) + x;
	return   sum.at<int>(img_y + HALF_KERNEL + 1, img_x + HALF_KERNEL + 1)
		- sum.at<int>(img_y + HALF_KERNEL + 1, img_x - HALF_KERNEL)
		- sum.at<int>(img_y - HALF_KERNEL, img_x + HALF_KERNEL + 1)
		+ sum.at<int>(img_y - HALF_KERNEL, img_x - HALF_KERNEL);
}

void pixelTests32(cv::Mat sum, std::vector<cv::KeyPoint> keypoints, cv::Mat descriptors, bool use_orientation)
{
	cv::Matx21f R;
	for (size_t i = 0; i < keypoints.size(); ++i)
	{
		uchar* desc = descriptors.ptr(static_cast<int>(i));
		cv::KeyPoint pt = keypoints[i];
		if (use_orientation)
		{
			float angle = pt.angle;
			angle *= (float)(CV_PI / 180.f);
			R(0, 0) = sin(angle);
			R(1, 0) = cos(angle);
		}

#include "generated_32.i"
	}
}


void my_brief_des(cv::Mat image, std::vector<cv::KeyPoint> keypoints, int bytes = 32, bool use_orientation = false)
{
	cv::Mat sum;
	cv::integral(image, sum, CV_32S);
	cv::KeyPointsFilter::runByImageBorder(keypoints, image.size(), PATCH_SIZE / 2 + KERNEL_SIZE / 2);

	cv::Mat descriptors;
	descriptors.create((int)keypoints.size(), bytes, CV_8U);
	descriptors.setTo(cv::Scalar::all(0));

	pixelTests32(sum, keypoints, descriptors, use_orientation);
	std::cout << descriptors << std::endl;
}


int main()
{
	cv::Mat img = cv::imread("susan_input1.png", 0);
	cv::Ptr<cv::FastFeatureDetector> fast = cv::FastFeatureDetector::create();
	std::vector<cv::KeyPoint> keypoints;
	fast->detect(img, keypoints);

	my_brief_des(img, keypoints);

	return 0;
}