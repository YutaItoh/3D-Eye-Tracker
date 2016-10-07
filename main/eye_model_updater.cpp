#include "eye_model_updater.h"

namespace eye_tracker{

// Some utility functions
cv::Point2f toImgCoord(const cv::Point2f& point, const cv::Mat& m, double scale , int shift ) {
	return cv::Point2f(static_cast<float>((m.cols / 2 + scale*point.x) * (1 << shift)),
		static_cast<float>((m.rows / 2 + scale*point.y) * (1 << shift)));
}
cv::Point toImgCoord(const cv::Point& point, const cv::Mat& m, double scale, int shift ) {
	return cv::Point(static_cast<int>((m.cols / 2 + scale*point.x) * (1 << shift)),
		static_cast<int>((m.rows / 2 + scale*point.y) * (1 << shift)));
}
cv::RotatedRect toImgCoord(const cv::RotatedRect& rect, const cv::Mat& m, float scale) {
	return cv::RotatedRect(toImgCoord(rect.center, m, scale),
		cv::Size2f(scale*rect.size.width,
		scale*rect.size.height),
		rect.angle);
}

cv::Point2f toImgCoordInv(const cv::Point2f& point, const cv::Mat& m, double scale , int shift ) {
	return cv::Point2f(
		static_cast<float>((point.x / (1 << shift) - m.cols / 2) / scale),
		static_cast<float>((point.y / (1 << shift) - m.rows / 2) / scale)
		);
}
cv::Point toImgCoordInv(const cv::Point& point, const cv::Mat& m, double scale, int shift) {
	return cv::Point(
		static_cast<int>((point.x / (1 << shift) - m.cols / 2) / scale),
		static_cast<int>((point.y / (1 << shift) - m.rows / 2) / scale)
		);
}
cv::RotatedRect toImgCoordInv(const cv::RotatedRect& rect, const cv::Mat& m, float scale) {
	return cv::RotatedRect(toImgCoordInv(rect.center, m, scale),
		cv::Size2f(rect.size.width/scale,
		rect.size.height/scale),
		rect.angle);
}

void space_bin_searcher_test()
{
	int w = 640;
	int h = 480;
	cv::Mat img = cv::Mat::zeros(cv::Size(w, h), CV_8UC3);
	eye_tracker::SpaceBinSearcher space_bin_searcher;
	space_bin_searcher.initialize(w, h);

	cv::Vec2i pt;
	const int N = 500;
	cv::Mat ClusterCenters(cvSize(2, N), CV_32S); // The set B
	cv::randu(ClusterCenters, cv::Scalar::all(0), cv::Scalar::all(640));
	for (int n = 0; n < N; n++){
		cv::Vec2i pt0(ClusterCenters.at<int>(n, 0), ClusterCenters.at<int>(n, 1));
		int x = pt0[0];
		int y = pt0[1];
		float dst;

		if (space_bin_searcher.search(x, y, pt, dst)){
			cv::circle(img, pt0, 3, cv::Vec3b(0, 255, 0));
		}
		else{
			cv::circle(img, pt0, 3, cv::Vec3b(0, 0, 255));
		}

		space_bin_searcher.render(img);
		//		cv::imshow("img", img);
		//	cv::waitKey(1);
	}

	return;
}
SpaceBinSearcher::SpaceBinSearcher()
	:kSearchGridSize_(16) {
}
SpaceBinSearcher::SpaceBinSearcher(int w, int h)
	: kSearchGridSize_(16){
	initialize(w, h);
}

void SpaceBinSearcher::initialize(int w, int h){

	if (is_initialized_ == true){
		std::cout << "SpaceBinSearcher::initialize: search tree is already initialized" << std::endl;
		return;
	}

	if (w < 0 || h < 0){
		std::cout << "SpaceBinSearcher: Map size must be positive" << std::endl;
		throw;
	}
	const int w_num = w / kSearchGridSize_;
	const int h_num = h / kSearchGridSize_;

	// Create matrices
	ClusterMembers_.create(cvSize(2, kN_), CV_32S); // The set A
	sample_num_ = (w_num + 1)*(h_num + 1);
	taken_flags_.resize(sample_num_);
	std::fill(taken_flags_.begin(), taken_flags_.end(), false);

	ClusterCenters_.create(cvSize(2, sample_num_), CV_32S); // The set B
	int idx = 0;
	for (int r = 0; r < h; r += kSearchGridSize_){
		for (int c = 0; c < w; c += kSearchGridSize_){
			ClusterCenters_.at<int>(idx, 0) = c;
			ClusterCenters_.at<int>(idx, 1) = r;
			idx++;

		}
	}

	kdtrees = new cv::flann::GenericIndex< cvflann::L2<int> >(ClusterCenters_, cvflann::KDTreeIndexParams(4)); // a 4 k-d tree
	is_initialized_ = true;
}
SpaceBinSearcher::~SpaceBinSearcher(){
	if (is_initialized_){
		delete kdtrees;
	}
}

void SpaceBinSearcher::render(cv::Mat &img){
	if (is_initialized_ == false){
		std::cout << "SpaceBinSearcher::render: search tree is not initialized" << std::endl;
		return;
	}
	if (img.empty()){
		std::cout << "SpaceBinSearcher::render: input image is empty" << std::endl;
		return;
	}
	cv::Rect bb(cv::Point(), img.size());
	for (int idx = 0; idx < sample_num_; idx++){
		cv::Vec2i center(ClusterCenters_.at<int>(idx, 0),
			ClusterCenters_.at<int>(idx, 1));
		const int  radius = 1;
		if (bb.contains(center)){
			if (taken_flags_[idx]){
				img.at<cv::Vec3b>(center[1], center[0]) = cv::Vec3b(0, 0, 255); // sample taken at least once
			}
			else{
				img.at<cv::Vec3b>(center[1], center[0]) = cv::Vec3b(0, 255, 0); // newly taken
			}
		}
	}
}
void SpaceBinSearcher::reset_indices(){
	std::fill(taken_flags_.begin(), taken_flags_.end(), false);
}

bool SpaceBinSearcher::search(int x, int y, cv::Vec2i &pt, float &dist){
	if (is_initialized_ == false){
		std::cout << "SpaceBinSearcher::search: search tree is not initialized" << std::endl;
		throw;
	}

	ClusterMembers_.at<int>(0, 0) = x;
	ClusterMembers_.at<int>(0, 1) = y;

	cv::Mat matches; //This mat will contain the index of nearest neighbour as returned by Kd-tree
	cv::Mat distances; //In this mat Kd-Tree return the distances for each nearest neighbour
	matches.create(cvSize(1, kN_), CV_32SC1);
	distances.create(cvSize(1, kN_), CV_32FC1);

	// Search KdTree
	kdtrees->knnSearch(ClusterMembers_, matches, distances, 1, cvflann::SearchParams(8));
	int NN_index;
	for (int i = 0; i < kN_; i++) {
		NN_index = matches.at<int>(i, 0);
		dist = distances.at<float>(i, 0);
		pt = ClusterCenters_.row(NN_index);
		if (taken_flags_[NN_index]){
			return false; // sample is taken already
		}
		else{
			taken_flags_[NN_index] = true;
			return true; // newly searched point
		}
	}
}

EyeModelUpdater::EyeModelUpdater(){

}

EyeModelUpdater::EyeModelUpdater(double focal_length, double region_band_width, double region_step_epsilon)
	: focal_length_(focal_length), simple_fitter_(focal_length_, region_band_width, region_step_epsilon),
	fitter_max_count_(kFitterMaxCountDefault_)
{
}

void EyeModelUpdater::add_fitter_max_count(int n){
	if (n <= 0) return;
	fitter_max_count_ += n;
	if (is_model_built_){
		is_model_built_ = false;
	}
}

bool EyeModelUpdater::add_observation(cv::Mat &image, sef::Ellipse2D<double> &pupil, std::vector<cv::Point2f> &pupil_inliers,bool force){
	if (space_bin_searcher_.is_initialized() == false){
		space_bin_searcher_.initialize(image.cols, image.rows);
	}
	bool is_added = false;
	if (force||(is_model_built_ == false && fitter_count_ < fitter_max_count_)){
		cv::Vec2i pt;
		float dist;
		// Check if we already added a 2D ellipse close to the current 2D ellipse given
		if (force||space_bin_searcher_.search(
			(int)(pupil.centre.x() + image.cols / 2), 
			(int)(pupil.centre.y() + image.rows / 2), pt, dist)){
			simple_fitter_.add_observation(image, pupil, pupil_inliers);
			fitter_count_++;
			if (fitter_count_ == fitter_max_count_){
				simple_fitter_.unproject_observations();
				simple_fitter_.initialise_model();
				is_model_built_ = true;
			}
			is_added = true;
		}
	}
	return is_added;
}

singleeyefitter::EyeModelFitter::Circle  EyeModelUpdater::unproject(cv::Mat &img, sef::Ellipse2D<double> &el, std::vector<cv::Point2f> &inlier_pts){
	if (simple_fitter_.eye){
		// Unproject the current 2D ellipse observations
		singleeyefitter::EyeModelFitter::Observation curr_obs(img, el, inlier_pts);
		singleeyefitter::EyeModelFitter::Pupil curr_pupil(curr_obs);
//		try{
//			if (curr_pupil.init_valid){
				//			singleeyefitter::EyeModelFitter::Circle curr_circle =
				simple_fitter_.unproject_single_observation(curr_pupil, simple_fitter_.eye.radius);
				singleeyefitter::EyeModelFitter::Circle curr_circle = simple_fitter_.initialise_single_observation(curr_pupil);

				return curr_circle;
//			}
		//}
		//catch (...){
		//	return singleeyefitter::EyeModelFitter::Circle::Null;
		//}
	}
	return singleeyefitter::EyeModelFitter::Circle::Null;
}


double EyeModelUpdater::compute_reliability(cv::Mat &img, sef::Ellipse2D<double> &el, std::vector<cv::Point2f> &inlier_pts){
	double realiabiliy = 0.0;
	if (simple_fitter_.eye){

		// Unproject the current 2D ellipse observation to a 3D disk
		singleeyefitter::EyeModelFitter::Circle curr_circle = unproject(img, el, inlier_pts);

		if (curr_circle && !isnan(curr_circle.normal(0, 0))){		
			const double displayscale = 1.0;
			singleeyefitter::Ellipse2D<double> pupil_el(sef::project(curr_circle, focal_length_));
			realiabiliy = el.similarity(pupil_el);

			//// 3D eyeball
			//cv::RotatedRect rr_eye = eye_tracker::toImgCoord(sef::toRotatedRect(sef::project(simple_fitter_.eye, focal_length_)), img, displayscale);

			//// 3D pupil
			//singleeyefitter::Ellipse2D<double> pupil_el(sef::project(curr_circle, focal_length_));
			//cv::RotatedRect rr_pupil = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(pupil_el), img, displayscale);

			//// 3D gaze vector
			//singleeyefitter::EyeModelFitter::Circle c_end = curr_circle;
			//c_end.centre = curr_circle.centre + (10.0)*curr_circle.normal; // Unit: mm
			//singleeyefitter::Ellipse2D<double> e_end(sef::project(c_end, focal_length_));
			//cv::RotatedRect rr_end = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(e_end), img, displayscale);

		}
	}
	return realiabiliy;
}

void EyeModelUpdater::render(cv::Mat &img, sef::Ellipse2D<double> &el, std::vector<cv::Point2f> &inlier_pts){

	if (simple_fitter_.eye){
		const float displayscale = 1.0f;

		// Unproject the current 2D ellipse observation to a 3D disk
		singleeyefitter::EyeModelFitter::Circle curr_circle = unproject(img, el, inlier_pts);

		if (curr_circle && !isnan(curr_circle.normal(0, 0))){
			// 3D eyeball
			cv::RotatedRect rr_eye = eye_tracker::toImgCoord(sef::toRotatedRect(sef::project(simple_fitter_.eye, focal_length_)), img, displayscale);
			cv::ellipse(img, rr_eye, cv::Vec3b(255, 128, 0), 1, CV_AA);
			cv::circle(img, rr_eye.center, 3, cv::Vec3b(255, 128, 0), 1); // Eyeball center projection

			// 3D pupil
			singleeyefitter::Ellipse2D<double> pupil_el(sef::project(curr_circle, focal_length_));
			cv::RotatedRect rr_pupil = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(pupil_el), img, displayscale);
			cv::ellipse(img, rr_pupil, cv::Vec3b(0, 255, 128), 1, CV_AA);
			cv::line(img, rr_eye.center, rr_pupil.center, cv::Vec3b(255, 128, 0), 1, CV_AA);

			// 3D gaze vector
			singleeyefitter::EyeModelFitter::Circle c_end = curr_circle;
			c_end.centre = curr_circle.centre + (10.0)*curr_circle.normal; // Unit: mm
			singleeyefitter::Ellipse2D<double> e_end(sef::project(c_end, focal_length_));
			cv::RotatedRect rr_end = eye_tracker::toImgCoord(singleeyefitter::toRotatedRect(e_end), img, displayscale);
			cv::line(img, cv::Point(rr_pupil.center), cv::Point(rr_end.center), cv::Vec3b(0, 255, 128), 2, CV_AA);

		}
	}
}

void EyeModelUpdater::reset(){
	simple_fitter_.reset();
	space_bin_searcher_.reset_indices();
	fitter_count_ = 0;
	is_model_built_ = false;
	fitter_max_count_ = kFitterMaxCountDefault_;
}

void EyeModelUpdater::render_status(cv::Mat &img){
	if (fitter_count_ > 0){
		for (auto pupil : simple_fitter_.pupils){
			if (pupil.init_valid){
				cv::ellipse(img, eye_tracker::toImgCoord(sef::toRotatedRect(pupil.observation.ellipse), img, 1), cv::Vec3b(0, 128, 255), 1, CV_AA);
			}
			else{
				cv::ellipse(img, eye_tracker::toImgCoord(sef::toRotatedRect(pupil.observation.ellipse), img, 1), cv::Vec3b(128, 0, 0), 1, CV_AA);
			}
		}
	}
	space_bin_searcher_.render(img);
}

void EyeModelUpdater::render_initialize_status(cv::Mat &img){
	if (is_status_initialized_ == false){
		is_status_initialized_ = true;
	}
}

}