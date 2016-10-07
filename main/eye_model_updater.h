#ifndef EYE_MODEL_UPDATER_H
#define EYE_MODEL_UPDATER_H


//#define NOMINMAX

#include <vector>
#include <algorithm>

#include <Eigen/Core>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/flann/flann.hpp>


//#include <pupiltracker/pupiltracker.h>
#include <singleeyefitter/singleeyefitter.h>
#include <singleeyefitter/intersect.h>
#include <singleeyefitter/projection.h>
#include <singleeyefitter/cvx.h>
#include <singleeyefitter/Circle.h>
#include <singleeyefitter/Ellipse.h>
#include <singleeyefitter/Sphere.h>


//#include "eye_util.h"


namespace eye_tracker{


	
// Some utility functions
cv::Point2f     toImgCoord(const cv::Point2f& point, const cv::Mat& m, double scale = 1, int shift = 0);
cv::Point       toImgCoord(const cv::Point&   point, const cv::Mat& m, double scale = 1, int shift = 0);
cv::RotatedRect toImgCoord(const cv::RotatedRect& rect, const cv::Mat& m, float scale = 1);
cv::Point2f     toImgCoordInv(const cv::Point2f& point, const cv::Mat& m, double scale = 1, int shift = 0);
cv::Point       toImgCoordInv(const cv::Point&   point, const cv::Mat& m, double scale = 1, int shift = 0);
cv::RotatedRect toImgCoordInv(const cv::RotatedRect& rect, const cv::Mat& m, float scale = 1);

namespace sef = singleeyefitter;

void space_bin_searcher_test();


/**
* @class SpaceBinSearcher
* @brief IA support class to sample 2D ellipse observation uniformly over the 2D image space 
*/
class SpaceBinSearcher
{
public:
	SpaceBinSearcher();
	SpaceBinSearcher(int w, int h);
		
	void initialize(int w, int h);
	~SpaceBinSearcher();

	void render(cv::Mat &img);
	void reset_indices();

	bool search(int x, int y, cv::Vec2i &pt, float &dist);
	bool is_initialized(){ return is_initialized_; };
protected:
	// Local variables initialized at the constructor
	const int kSearchGridSize_;
	cv::Mat ClusterMembers_; //This Set A
	cv::Mat ClusterCenters_;  //This set B
	cv::flann::GenericIndex< cvflann::L2<int> > *kdtrees; // The flann searching tree

	// Local variables 
	bool is_initialized_ = false;
	const int kN_ = 1;
	int sample_num_;
	std::vector<bool> taken_flags_;
};

// 3D eye model fitting
class EyeModelUpdater
{
public:
	EyeModelUpdater();
	EyeModelUpdater(double focal_length, double region_band_width, double region_step_epsilon);

	bool add_observation(cv::Mat &image, sef::Ellipse2D<double> &pupil, std::vector<cv::Point2f> &pupil_inliers, bool force=false);
	
	singleeyefitter::EyeModelFitter::Circle unproject(cv::Mat &img, sef::Ellipse2D<double> &el, std::vector<cv::Point2f> &inlier_pts);
	
	double compute_reliability(cv::Mat &img, sef::Ellipse2D<double> &el, std::vector<cv::Point2f> &inlier_pts);

	void render(cv::Mat &img, sef::Ellipse2D<double> &el, std::vector<cv::Point2f> &inlier_pts);

	void reset();

	void render_status(cv::Mat &img);
	void render_initialize_status(cv::Mat &img);

	~EyeModelUpdater(){}
	bool is_model_built(){ return is_model_built_; }
	size_t fitter_count(){ return fitter_count_; }
	size_t fitter_end_count(){ return fitter_max_count_; }
	void add_fitter_max_count(int n);
	const singleeyefitter::EyeModelFitter&fitter(){ return simple_fitter_; };
protected:
	// Local variables initialized at the constructor
	double focal_length_;
	singleeyefitter::EyeModelFitter simple_fitter_;

	// Local variables 
	static const size_t kFitterMaxCountDefault_ = 30;// 100;
	size_t fitter_count_ = 0;
	size_t fitter_max_count_ = kFitterMaxCountDefault_;// 100;
	bool is_model_built_ = false;
	bool is_status_initialized_ = false;
	SpaceBinSearcher space_bin_searcher_;

private:
	// Prevent copying
//	EyeModelUpdater(const EyeModelUpdater& other);
//	EyeModelUpdater& operator=(const EyeModelUpdater& rhs);
};

}
#endif // EYE_MODEL_UPDATER_H