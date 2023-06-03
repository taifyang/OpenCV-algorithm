#include <iostream>
#include <opencv2/opencv.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <numeric>


int kBytes = 32;
int nfeatures = 500;
float scaleFactor = 1.2;
int nlevels = 8;
int edgeThreshold = 31;
int firstLevel = 0;
int wta_k = 2;
int scoreType = 0;
int patchSize = 31;
int fastThreshold = 20;
float harris_k = 0.04;


float getScale(int level, int firstLevel, float scaleFactor)
{
	return pow(scaleFactor, level - firstLevel);
}


template <typename T>
std::vector<size_t> sort_indices(const std::vector<T>& v) 
{
	std::vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);
	std::sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });
	return idx;
}


std::vector<cv::KeyPoint> retainBest(std::vector<cv::KeyPoint> keypoints, int n_points)
{
	if (n_points >= 0 && keypoints.size() > n_points)
	{
		std::vector<float> res;
		for (auto kp : keypoints)
			res.push_back(kp.response);
		std::vector<size_t> index = sort_indices(res);
		std::reverse(index.begin(), index.end());
		std::vector<cv::KeyPoint> new_keypoints;
		for (size_t i = 0; i < n_points; i++)
			new_keypoints.push_back(keypoints[index[i]]);
		return new_keypoints;
	}
	return keypoints;
}


void HarrisResponses(cv::Mat img, std::vector<std::vector<int>> layerinfo, std::vector<cv::KeyPoint>& pts, int blockSize)
{
	int radius = int(blockSize / 2);
	float scale = 1.0 / ((1 << 2) * blockSize * 255.0);
	float scale_sq_sq = pow(scale, 4);

	for (size_t i = 0; i < pts.size(); i++)
	{
		int x0 = int(round(pts[i].pt.x));
		int y0 = int(round(pts[i].pt.y));
		int z = int(pts[i].octave);

		int center_c = layerinfo[z][0] + x0;
		int center_r = layerinfo[z][1] + y0;
		long long a = 0, b = 0, c = 0;
		for (int index_r = - radius; index_r < blockSize - radius; index_r++)
		{
			for (int index_c = - radius; index_c < blockSize - radius; index_c++)
			{
				int rr = center_r + index_r;
				int cc = center_c + index_c;

				int Ix = (img.at<uchar>(rr, cc + 1) - img.at<uchar>(rr, cc - 1)) * 2 +
					(img.at<uchar>(rr - 1, cc + 1) - img.at<uchar>(rr - 1, cc - 1)) +
					(img.at<uchar>(rr + 1, cc + 1) - img.at<uchar>(rr + 1, cc - 1));

				int Iy = (img.at<uchar>(rr + 1, cc ) - img.at<uchar>(rr - 1, cc )) * 2 +
					(img.at<uchar>(rr + 1, cc - 1) - img.at<uchar>(rr - 1, cc - 1)) +
					(img.at<uchar>(rr + 1, cc + 1) - img.at<uchar>(rr - 1, cc + 1));

				a += Ix * Ix;
				b += Iy * Iy;
				c += Ix * Iy;
			}
		}
		pts[i].response = (a * b - c * c - harris_k * (a + b) * (a + b)) * scale_sq_sq;
	}
}


void ICAngles(cv::Mat img, std::vector<std::vector<int>> layerinfo, std::vector<cv::KeyPoint>& pts, std::vector<int> u_max, int half_k)
{
	for (size_t i = 0; i < pts.size(); i++)
	{
		std::vector<int> layer = layerinfo[pts[i].octave];
		int c = round(pts[i].pt.x) + layer[0], r = round(pts[i].pt.y) + layer[1];
		int m_01 = 0, m_10 = 0;

		for (int u = -half_k; u < half_k + 1; u++)
			m_10 += u * img.at<uchar>(r, c + u);

		for (int v = 1; v < half_k + 1; v++)
		{
			int v_sum = 0, d = u_max[v];
			for (int u = - d; u < d + 1; u++)
			{
				int val_plus= img.at<uchar>(r + v, c + u);
				int val_minus = img.at<uchar>(r - v, c + u);
				v_sum += (val_plus - val_minus);
				m_10 += u * (val_plus + val_minus);
			}
			m_01 += v * v_sum;
		}

		pts[i].angle = cv::fastAtan2(float(m_01), float(m_10));
	}
}


std::vector<cv::KeyPoint> computeKeyPoints(cv::Mat imagePyramid, std::vector<std::vector<int>> layerInfo, std::vector<float> layerScale)
{

	int nlevels = layerInfo.size();
	std::vector<int> nfeaturesPerLevel;
	float factor = float(1.0 / scaleFactor);
	float ndesiredFeaturesPerScale = nfeatures * (1 - factor) / (1 - pow(factor, nlevels));
	int sumFeatures = 0;
	for (size_t level = 0; level < nlevels - 1; level++)
	{
		nfeaturesPerLevel.push_back(int(round(ndesiredFeaturesPerScale)));
		sumFeatures += nfeaturesPerLevel[level];
		ndesiredFeaturesPerScale *= factor;
	}
	nfeaturesPerLevel.push_back(std::max(nfeatures - sumFeatures, 0));
	int halfPatchSize = int(patchSize / 2);
	std::vector<cv::KeyPoint> allKeypoints;
	std::vector<int> counters;

	for (size_t level = 0; level < nlevels; level++)
	{
		int featuresNum = int(nfeaturesPerLevel[level]);
		std::vector<int> r = layerInfo[level];
		cv::Mat img = imagePyramid(cv::Rect(r[0], r[1], r[2], r[3]));
		cv::Ptr<cv::FastFeatureDetector> fast = cv::FastFeatureDetector::create(fastThreshold);
		std::vector<cv::KeyPoint> keypoints;
		fast->detect(img, keypoints);
		cv::KeyPointsFilter::runByImageBorder(keypoints, img.size(), edgeThreshold);
		keypoints = retainBest(keypoints, 2 * featuresNum);
		int nkeypoints = keypoints.size();
		counters.push_back(nkeypoints);
		float sf = layerScale[level];
		for (size_t i = 0; i < nkeypoints; i++)
		{
			keypoints[i].octave = level;
			keypoints[i].size = patchSize * sf;
		}
		for (auto kp : keypoints)
			allKeypoints.push_back(kp);
	}
	int nkeypoints = allKeypoints.size();
	if (nkeypoints == 0)
		return {};

	HarrisResponses(imagePyramid, layerInfo, allKeypoints, 7);
	std::vector<cv::KeyPoint> newAllKeypoints;
	int offset = 0;

	for (size_t level = 0; level < nlevels; level++)
	{
		std::vector<cv::KeyPoint> keypoints;
		int featuresNum = int(nfeaturesPerLevel[level]);
		int nkeypoints = int(counters[level]);
		for (size_t i = offset; i < offset + nkeypoints; i++)
			keypoints.push_back(allKeypoints[i]);
		offset += nkeypoints;
		std::vector<cv::KeyPoint> keypoints2 = retainBest(keypoints, featuresNum);
		for(auto kp: keypoints2)
			newAllKeypoints.push_back(kp);
	}

	allKeypoints = newAllKeypoints;
	nkeypoints = allKeypoints.size();

	std::vector<int> umax(halfPatchSize + 2, 0);
	int vmax = int(floor((halfPatchSize * sqrt(2.) / 2 + 1)));
	for (int v = 0; v < vmax + 1; ++v)
		umax[v] = round(sqrt(double(pow(halfPatchSize, 2) - pow(v, 2))));
	int vmin = ceil(halfPatchSize * sqrt(2.0) / 2);

	int v = halfPatchSize, v0 = 0;
	while (v >= vmin)
	{
		while (umax[v0] == umax[v0 + 1])
			v0 = v0 + 1;
		umax[v] = v0;
		v0 = v0 + 1;
		v = v - 1;
	}

	ICAngles(imagePyramid, layerInfo, allKeypoints, umax, halfPatchSize);

	for (size_t i = 0; i < nkeypoints; i++)
	{
		float scale = layerScale[allKeypoints[i].octave];
		allKeypoints[i].pt = cv::Point(allKeypoints[i].pt.x * scale, allKeypoints[i].pt.y * scale);
	}
	return allKeypoints;
}


float GET_VALUE(int idx, std::vector<cv::Point> pattern, float a, float b, cv::Mat imagePyramid, int cx, int cy)
{
	float x = pattern[idx].x * a - pattern[idx].y * b;
	float y = pattern[idx].x * b + pattern[idx].y * a;
	int ix = int(round(x));
	int iy = int(round(y));
	return imagePyramid.at<float>(cy + iy, cx + ix);
}


std::vector<std::vector<int>> computeOrbDescriptors(cv::Mat imagePyramid, std::vector<std::vector<int>> layerInfo,
	std::vector<float> layerScale, std::vector<cv::KeyPoint> keypoints, std::vector<cv::Point> _pattern, int dsize)
{
	std::vector<std::vector<int>> descriptors;
	for (size_t i = 0; i < keypoints.size(); i++)
	{
		cv::KeyPoint kpt = keypoints[i];
		std::vector<int> layer = layerInfo[int(kpt.octave)];
		float scale = 1.0 / layerScale[int(kpt.octave)];
		float angle = kpt.angle / 180.0 * M_PI;
		float a = cos(angle), b = sin(angle);

		int cx = round(kpt.pt.x * scale) + layer[0], cy = round(kpt.pt.y * scale) + layer[1];
		int pattern_idx = 0;

		std::vector<cv::Point> pattern;
		pattern.assign(_pattern.begin()+ pattern_idx, _pattern.end());
		std::vector<int> des;
		for (size_t j = 0; j < dsize; j++)
		{
			int byte_v = 0;
			for (size_t nn = 0; nn < 8; nn++)
			{
				float t0 = GET_VALUE(2 * nn, pattern, a, b, imagePyramid, cx, cy);
				float t1 = GET_VALUE(2 * nn + 1, pattern, a, b, imagePyramid, cx, cy);
				int bit_v = int(t0 < t1);
				byte_v += (bit_v << nn);
			}
			des.push_back(byte_v);
			pattern_idx += 16;
			pattern.assign(_pattern.begin() + pattern_idx, _pattern.end());
		}
		descriptors.push_back(des);
	}
	return descriptors;
}


std::tuple<std::vector<cv::KeyPoint>, std::vector<std::vector<int>>> orb_detectAndCompute(cv::Mat img_src)
{
	int HARRIS_BLOCK_SIZE = 9;
	int halfPatchSize = int(patchSize / 2);
	int descPatchSize = int(ceil(halfPatchSize * sqrt(2.0)));
	std::vector<int> vec = { edgeThreshold, descPatchSize, HARRIS_BLOCK_SIZE / 2 };
	int border = int(*std::max_element(vec.begin(),vec.end()) + 1);
	cv::Mat image = img_src.clone();

	std::vector<float> layerScale;
	std::vector<std::vector<int>> layerInfo;
	int level_dy = (int)(image.cols + border * 2);
	cv::Point level_ofs(0, 0);
	int tmp = round(image.cols / getScale(0, firstLevel, scaleFactor)) + border * 2 + 15;
	cv::Size bufSize(tmp / 16 * 16, 0);

	for (size_t level = 0; level < nlevels; level++)
	{
		float scale = getScale(level, firstLevel, scaleFactor);
		layerScale.push_back(scale);
		cv::Size sz(int(round(image.cols / scale)), int(round(image.rows / scale)));
		cv::Size wholeSize(sz.width + border * 2, sz.height + border * 2);
		if (level_ofs.x + wholeSize.width > bufSize.width)
		{
			level_ofs = cv::Point(0, level_ofs.y + level_dy);
			level_dy = wholeSize.height;
		}
		std::vector<int> linfo = { level_ofs.x + border, level_ofs.y + border, sz.width, sz.height };
		layerInfo.push_back(linfo);
		level_ofs.x += wholeSize.width;
	}
	bufSize.height = level_ofs.y + level_dy;

	cv::Mat imagePyramid(bufSize, CV_8UC1);

	cv::Mat prevImg = image;
	for (size_t level = 0; level < nlevels; level++)
	{
		std::vector<int> linfo = layerInfo[level];
		cv::Size sz(linfo[2],linfo[3]);
		cv::Size wholeSize(sz.width + border * 2, sz.height + border * 2);
		std::vector<int> wholeLinfo = { linfo[0] - border, linfo[1] - border, wholeSize.width, wholeSize.height };
		cv::Mat extImg, currImg;
		if (level == firstLevel)
		{
			cv::copyMakeBorder(image, extImg, border, border, border, border, cv::BORDER_REFLECT_101);
		}
		else 
		{
			cv::resize(prevImg, currImg, sz, 0, 0, cv::INTER_LINEAR_EXACT);
			cv::copyMakeBorder(currImg, extImg, border, border, border, border, cv::BORDER_REFLECT_101 + cv::BORDER_ISOLATED);
		}
		cv::Mat roi = imagePyramid(cv::Rect(wholeLinfo[0], wholeLinfo[1], wholeLinfo[2], wholeLinfo[3]));
		extImg.copyTo(roi);
		if (level > firstLevel)
			prevImg = currImg.clone();
	}

	imagePyramid = cv::imread("imagePyramid.png", 0);

	std::vector<cv::KeyPoint> keypoints = computeKeyPoints(imagePyramid, layerInfo, layerScale);

	int dsize = kBytes;
	int nkeypoints = keypoints.size();
	int npoints = 512;

#include "bit_pattern_31.i"

	std::vector<cv::Point> pattern;
	for (size_t i = 0; i < npoints; i++)
		pattern.push_back(cv::Point(bit_pattern_31_[2 * i], bit_pattern_31_[2 * i + 1]));

	imagePyramid.convertTo(imagePyramid, CV_32F);
	for (size_t level = 0; level < nlevels; level++)
	{
		int x = layerInfo[level][0], y = layerInfo[level][1], w = layerInfo[level][2], h = layerInfo[level][3];
		cv::Mat workingMat = imagePyramid(cv::Rect(x, y, w, h));
		workingMat.convertTo(workingMat, CV_32F);
		cv::GaussianBlur(workingMat, workingMat, cv::Size(7, 7), 2, 2, cv::BORDER_REFLECT_101);
	}
	//cv::imwrite("imagePyramid.png", imagePyramid);

	std::vector<std::vector<int>> descriptors = computeOrbDescriptors(imagePyramid, layerInfo, layerScale, keypoints, pattern, dsize);

	return { keypoints, descriptors };
}


int main()
{
	cv::Mat img = cv::imread("1.png", 0);

	auto [kp, des] = orb_detectAndCompute(img);

	return 0;
}