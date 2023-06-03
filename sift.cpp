#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>
#define _USE_MATH_DEFINES
#include <math.h>


class Sift
{
public:
	Sift(cv::Mat img)
	{
		m_img = img.clone();
		m_r = m_img.rows, m_c = m_img.cols;
		m_gradient = cv::Mat::zeros(m_r, m_c, CV_32F);
		m_angle = cv::Mat::zeros(m_r, m_c, CV_32F);
	};

	std::tuple<cv::Mat, cv::Mat, int> get_result()
	{
		cv::goodFeaturesToTrack(m_img, m_corners, 233, 0.01, 10);
		//std::cout << corners << std::endl;

		cv::GaussianBlur(m_img, m_img, cv::Size(5, 5), 1, 1);
		m_img.convertTo(m_img, CV_32F);

		grad(m_img);

		m_bins = (m_r + m_c) / 80;
		m_length = m_corners.rows;
		m_feature = cv::Mat::zeros(m_length, 128, CV_32F);

		vote();

		for (size_t i = 0; i < m_length; i++)
		{
			cv::Point2f p(m_corners.at<float>(i, 1), m_corners.at<float>(i, 0));
			std::vector<int> val = get_feature(p, m_direct[i]);
			float m = 0;
			for (float k : val)
				m += k * k;
			m = sqrt(m);
			std::vector<float> l;
			for (float k : val)
				l.push_back(k / m);
			cv::Mat temp_row = cv::Mat(l).reshape(1, 1);
			//std::cout << temp_row << std::endl;
			temp_row.copyTo(m_feature.row(i));
		}
		//std::cout << m_feature << std::endl;
		return { m_feature, m_corners, m_length };
	}

	void grad(cv::Mat img)
	{
		int x = m_r, y = m_c;

		cv::Mat kernel_x = (cv::Mat_<float>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1) / 6;
		cv::Mat kernel_y = (cv::Mat_<float>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1) / 6;

		cv::Mat gx, gy;
		cv::filter2D(img, gx, -1, kernel_x);
		cv::filter2D(img, gy, -1, kernel_y);
		//std::cout << gx.at<float>(1, 0) << std::endl;
		//std::cout << gy.at<float>(1, 0) << std::endl;

		for (size_t i = 0; i < x; i++)
		{
			for (size_t j = 0; j < y; j++)
			{
				m_gradient.at<float>(i, j) = sqrt(pow(gx.at<float>(i, j), 2) + pow(gy.at<float>(i, j), 2));
				m_angle.at<float>(i, j) = atan2(gy.at<float>(i, j), gx.at<float>(i, j));
			}
		}
		//std::cout << gradient.at<float>(0, 1) << std::endl;
		//std::cout << angle.at<float>(0, 1) << std::endl;
	}

	void vote()
	{
		for (size_t n = 0; n < m_length; n++)
		{
			int y = m_corners.at<float>(n, 0), x = m_corners.at<float>(n, 1);

			std::vector<int> voting(37);

			for (size_t i = std::max(x - m_bins, 0); i < std::min(x + m_bins + 1, m_r); i++)
			{
				for (size_t j = std::max(y - m_bins, 0); j < std::min(y + m_bins + 1, m_c); j++)
				{
					int k = int((m_angle.at<float>(i, j) + M_PI) / (M_PI / 18) + 1);
					if (k >= 37)
						k = 36;
					voting[k] += m_gradient.at<float>(i, j);
				}
			}

			int p = 1;
			for (size_t i = 2; i < 37; i++)
			{
				if (voting[i] > voting[p])
					p = i;
			}
			m_direct.push_back(float(p / 18.0 - 1 - 1.0 / 36) * M_PI);
		}
	}

	float get_theta(float x, float y)
	{
		if ((x < 0 || x >= m_r) || (y < 0 || y >= m_c))
			return 0;
		float dif = m_angle.at<float>(x, y) - m_theta;
		return dif > 0 ? dif : dif + 2 * M_PI;
	}

	float DB_linear(float x, float y)
	{
		int xx = int(x), yy = int(y);
		float dx1 = x - xx, dx2 = xx + 1 - x;
		float dy1 = y - yy, dy2 = yy + 1 - y;
		float val = get_theta(xx, yy) * dx2 * dy2 + get_theta(xx + 1, yy) * dx1 * dy2 + get_theta(xx, yy + 1) * dx2 * dy1 + get_theta(xx + 1, yy + 1) * dx1 * dy1;
		return val;
	}

	std::vector<int> cnt(int x1, int x2, int y1, int y2, int xsign, int ysign)
	{
		std::vector<int> voting(9);
		for (size_t x = x1; x < x2; x++)
		{
			for (size_t y = y1; y < y2; y++)
			{
				cv::Mat p = m_H * x * xsign + m_V * y * ysign;
				int bin = int((DB_linear(p.at<float>(0, 0) + m_pos.x, p.at<float>(0, 1) + m_pos.y)) / (M_PI / 4) + 1);
				if (bin > 8)
					bin = 8;
				voting[bin] += 1;
			}
		}
		std::vector<int> tmp(8);
		std::copy(voting.begin()+1, voting.end(), tmp.begin());
		return tmp;
	}

	std::vector<int> get_feature(cv::Point2f pos, float theta)
	{
		m_pos = pos;
		m_theta = theta;
		m_H = (cv::Mat_<float>(1, 2) << cos(m_theta), sin(m_theta));
		m_V = (cv::Mat_<float>(1, 2) << -sin(m_theta), cos(m_theta));

		m_bins = (m_r + m_c) / 150;
		std::vector<int> val;
		std::vector<int> tmp;
		for (int xsign = -1; xsign <= 1; xsign += 2)
		{
			for (int ysign = -1; ysign <= 1; ysign += 2)
			{
				tmp = cnt(0, m_bins, 0, m_bins, xsign, ysign);
				val.insert(val.end(), tmp.begin(), tmp.end());
				tmp = cnt(m_bins, m_bins * 2, 0, m_bins, xsign, ysign);
				val.insert(val.end(), tmp.begin(), tmp.end());
				tmp = cnt(m_bins, m_bins * 2, m_bins, m_bins * 2, xsign, ysign);
				val.insert(val.end(), tmp.begin(), tmp.end());
				tmp = cnt(0, m_bins, m_bins, m_bins * 2, xsign, ysign);
				val.insert(val.end(), tmp.begin(), tmp.end());
			}
		}
		return val;
	}

private:
	int m_r;
	int m_c;
	int m_bins;
	int m_length;
	float m_theta;
	cv::Mat m_img;
	cv::Mat m_corners;
	cv::Mat m_gradient;
	cv::Mat m_angle;
	cv::Mat m_H;
	cv::Mat m_V;
	cv::Mat m_feature;
	cv::Point2f m_pos;
	std::vector<float> m_direct;
};


cv::Mat merge(cv::Mat img1, cv::Mat img2)
{
	int h1 = img1.rows, w1 = img1.cols;
	int h2 = img2.rows, w2 = img2.cols;
	cv::Mat img;
	if (h1 < h2)
	{
		img = cv::Mat::zeros( h2, w1 + w2, CV_8UC3);
		cv::Mat roi = img(cv::Rect(0, 0, img1.cols, img1.rows));
		img1.copyTo(roi);
		roi = img(cv::Rect(img1.cols, 0, img2.cols, img2.rows));
		img2.copyTo(roi);
	}
	else if (h1 > h2)
	{
		img = cv::Mat::zeros(h1, w1 + w2, CV_8UC3);
		cv::Mat roi = img(cv::Rect(0, 0, img1.cols, img1.rows));
		img1.copyTo(roi);
		roi = img(cv::Rect(img1.cols, 0, img2.cols, img2.rows));
		img2.copyTo(roi);
	}
	return img;
}


int main(int argc, char** argv)
{
	cv::Mat src = cv::imread("source.jpg", 1), src_gray;
	cv::Mat tgt = cv::imread("target.jpg", 1), tgt_gray;

	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	cv::cvtColor(tgt, tgt_gray, cv::COLOR_BGR2GRAY);

	Sift sift_src(src_gray);
	Sift sift_tgt(tgt_gray);

	auto [fs ,cs ,ls] = sift_src.get_result();
	auto [ft, ct, lt] = sift_tgt.get_result();

	cv::Mat img = merge(tgt, src);

	for (size_t i = 0; i < lt; i++)
	{
		std::vector<float> tmp;
		for (size_t j = 0; j < ls; j++)
		{
			//std::cout << ft.row(i) << std::endl;
			//std::cout << fs.row(j) << std::endl;
			float sc = (ft.row(i)).dot(fs.row(j));
			tmp.push_back(sc);
		}

		int b = std::max_element(tmp.begin(), tmp.end()) - tmp.begin();
		float s = *std::max_element(tmp.begin(), tmp.end());

		if (s < 0.8)
			continue;

		cv::Scalar color(rand() % 255, rand() % 255, rand() % 255);
		cv::Point p_start(ct.at<float>(i, 0), ct.at<float>(i, 1));
		cv::Point p_end(cs.at<float>(b, 0) + tgt.cols, cs.at<float>(b, 1));
		cv::line(img, p_start, p_end, color, 1);
	}

	cv::imwrite("mysift.jpg", img);

	return EXIT_SUCCESS;
}