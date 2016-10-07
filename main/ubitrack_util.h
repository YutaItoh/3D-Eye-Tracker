#ifndef UBITRACK_UTIL_H
#define UBITRACK_UTIL_H

#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/thread.hpp>
#include <boost/math/quaternion.hpp>

#include <opencv2/core.hpp>



using boost::asio::ip::udp;
#define MAX_MASSAGE_BUFFER_SIZE 2048

namespace eye_tracker{

/// Timestamp: nanoseconds since epoch (UNIX birth)
typedef unsigned long long int Timestamp;

/// A dummy timestamp class to parse serialized Ubitrack message
class UbitrackTimestamp {
public:
	UbitrackTimestamp(){}
	Timestamp t;
private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, unsigned int /*version*/)
	{
		ar&t;
	}
};

/// A data class to parse serialized Ubitrack message
class Position3D {
public:
	double x, y, z;

	Position3D() :
		x(0), y(0), z(0){
	}

	Position3D(double x0, double y0, double z0) :
		x(x0), y(y0), z(z0){
	}

	Position3D(const Position3D &other)
		:x(other.x), y(other.y), z(other.z)
	{
	}

	Position3D &operator=(const Position3D &other)
	{
		if (this != &other) {
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}

	Position3D &operator+(const Position3D &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	
	void print()
	{
		std::cout << x << " " << y << " " << z << std::endl;
	}

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, unsigned int /*version*/)
	{
		ar & x;
		ar & y;
		ar & z;
	}
};

/// A data class to parse serialized Ubitrack message
class Quaternion 
	: public boost::math::quaternion<double>
{
public:

	double w() const { return a; }

	double x() const { return b; }
	double y() const { return c; }
	double z() const { return d; }

	Quaternion()
		: boost::math::quaternion< double >(1, 0, 0, 0)
	{
	}

	void print()
	{
		std::cout << this << std::endl;
	}

	// rotate a vector by a quaternion
	Position3D operator*(const Position3D& vec) const
	{
		Position3D r;

		// precomputation of some values
		double xy = x() * y();
		double xz = x() * z();
		double yz = y() * z();
		double ww = w() * w();
		double wx = w() * x();
		double wy = w() * y();
		double wz = w() * z();

		r.x = vec.x * (2 * (x()*x() + ww) - 1) + vec.y * 2 * (xy - wz) + vec.z * 2 * (wy + xz);
		r.y = vec.x * 2 * (xy + wz) + vec.y * (2 * (y()*y() + ww) - 1) + vec.z * 2 * (yz - wx);
		r.z = vec.x * 2 * (xz - wy) + vec.y * 2 * (wx + yz) + vec.z * (2 * (z()*z() + ww) - 1);

		return r;

	}
private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, unsigned int /*version*/)
	{
		ar & b;
		ar & c;
		ar & d;
		ar & a;
	}
	int dummy;
};



/// A data class to parse serialized Ubitrack message
class Pose {
public:
	Quaternion q;
	Position3D pos;

	Pose() :
		q(), pos(){
	};


	Position3D Pose::operator*(const Position3D& x) const
	{
		return Position3D((q * x) + pos);
	}

	void print()
	{
		q.print();
		pos.print();
	}

  Pose &operator=(const Pose &other)
  {
    // 自身の代入チェック
    if (this != &other) {
		q = other.q;
		pos = other.pos;
    }
    return *this;
  }

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, unsigned int /*version*/)
	{
		ar & q;
		ar & pos;
	}
};

class Caib{
	
public:
	double K[9];
	int w;
	int h;
	int rad_dis_num;
	double rad_dis[6];
	int tan_dis_num;
	double tan_dis[2];
	Caib(){
		for (int k = 0; k < 9; k++){
			K[k] = k + 0.1;
		}
		w = 640;
		h = 480;
		rad_dis_num = 6;
		for (int k = 0; k < rad_dis_num; k++){
			rad_dis[k] = k + 0.2;
		}
		tan_dis_num = 2;
		for (int k = 0; k < tan_dis_num; k++){
			tan_dis[k] = k + 0.3;
		}
	};
	
	// Convert parameters style From Ubitrack to OpenCV (image top-left)
	void get_parameters_opencv_default(cv::Mat &Kmat, cv::Vec<double, 8> &distCoeffs){
		Kmat = (cv::Mat_<double>(3, 3) <<
			K[0], K[1],    -K[2],
			K[3], K[4], h-1+K[5],
			K[6], K[7],    -K[8]);
		//cv::Vec<double, 8> distCoeffs(k1, k2, p1, p2, k3, k4, k5, k6); // (k1 k2 p1 p2 [k3 [k4 k5 k6]])
		distCoeffs[0] = rad_dis[0];
		distCoeffs[1] = rad_dis[1];
		distCoeffs[2] = tan_dis[0];
		distCoeffs[3] = -tan_dis[1]; /// for Opencv 
		distCoeffs[4] = rad_dis[2];
		distCoeffs[5] = rad_dis[3];
		distCoeffs[6] = rad_dis[4];
		distCoeffs[7] = rad_dis[5];
	}
	// Convert parameters style From Ubitrack to OpenGL (image top-left)
	void set_parameters_opengl(cv::Mat &Kmat, cv::Vec<double, 8> &distCoeffs){
		Kmat = (cv::Mat_<double>(3, 3) <<
			K[0], -K[1], K[2],
			K[3], -K[4], -(h-1+K[5]),
			K[6], -K[7], K[8]);
		//cv::Vec<double, 8> distCoeffs(k1, k2, p1, p2, k3, k4, k5, k6); // (k1 k2 p1 p2 [k3 [k4 k5 k6]])
		distCoeffs[0] = rad_dis[0];
		distCoeffs[1] = rad_dis[1];
		distCoeffs[2] = tan_dis[0];
		distCoeffs[3] = tan_dis[1];
		distCoeffs[4] = rad_dis[2];
		distCoeffs[5] = rad_dis[3];
		distCoeffs[6] = rad_dis[4];
		distCoeffs[7] = rad_dis[5];
	}
	void project(const Position3D &p, double &u, double &v){
		double x = K[0] * p.x + K[1] * p.y + K[2] * p.z;
		double y = K[3] * p.x + K[4] * p.y + K[5] * p.z;
		double z = K[6] * p.x + K[7] * p.y + K[8] * p.z;
		u = x / z;
		v = y / z;
	}
	void print(){
		std::cout << K[0] << " " << K[1] << " " << K[2] << std::endl;
		std::cout << K[3] << " " << K[4] << " " << K[5] << std::endl;
		std::cout << K[6] << " " << K[7] << " " << K[8] << std::endl;
	}

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, unsigned int /*version*/)
	{
		for (int k = 0; k < 9; k++){
			ar &K[k];
		}
		ar&w;
		ar&h;
		ar&rad_dis_num;
		for (int k = 0; k < rad_dis_num; k++){
			ar &rad_dis[k];
		}
		ar&tan_dis_num;
		for (int k = 0; k < tan_dis_num; k++){
			ar &tan_dis[k];
		}
	}
};

template <class DataType>
class UbitrackTextReader
{
public:
	bool read(std::string path){
		std::string line;
		std::ifstream myfile(path);

		if (myfile.is_open() == false) return false;

		getline(myfile, line);

		if (line.empty() ) return false;

		std::istringstream archive_stream(line);
		boost::archive::text_iarchive archive(archive_stream);
		boost::archive::text_oarchive ot(std::cout);
		ot << t_;
		ot << data_;
		try {
			archive >> t_;
			archive >> data_;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}
	UbitrackTimestamp t_;
	DataType data_;
private:

};

template <class DataType>
class UbitrackUDPClient
{
public:

	UbitrackUDPClient(unsigned short port)
		: io_service(),
		socket(io_service, { udp::v4(), port })
	{
	}
	void start(){
		do_receive();
		io_service.run();

	}
	void stop(){
		io_service.stop();
	}
	void get(DataType &data, Timestamp &timestamp){
		boost::mutex::scoped_lock lock(mtx_);
//		mutex_.lock();
		data = data_;
		timestamp = time_stamp_.t;
//		std::cout << data_.t << std::endl;
//		mutex_.unlock();
	}

private:
	void do_receive()
	{
		socket.async_receive_from(boost::asio::buffer(recv_buffer), receiver_endpoint,
			boost::bind(&UbitrackUDPClient::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
	{

		std::string input_data_(recv_buffer.data(), bytes_transferred);
		std::istringstream archive_stream(input_data_);
		boost::archive::text_iarchive archive(archive_stream);
		try {
			boost::mutex::scoped_lock lock(mtx_);
			archive >> parttrn_name_;
			archive >> time_stamp_;
			archive >> data_;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}

		if (!error || error == boost::asio::error::message_size)
			do_receive();
	}

	// Network stuffx
	boost::asio::io_service io_service;
	udp::socket socket;
	udp::endpoint receiver_endpoint;
	boost::array<char, 1024> recv_buffer;

	// Ubitrack data stuff
    boost::mutex mtx_;
	DataType data_;
	UbitrackTimestamp time_stamp_;
	std::string parttrn_name_;
};


//void ubitrack_network_receiver_test(){
//
//	std::string s = "pattern_2";
//	UbitrackTimestamp t;
//	Position3D data;
//	long timestamp = 1471776266586383032L;
//
//	boost::archive::text_oarchive ot(std::cout);
//	ot << s;
//	ot << t;
//	ot << data;
//	const unsigned short port = 21844;
//	UbitrackUDPClient<Position3D> ubitrack_network_receiver(port);
//}

}
#endif // UBITRACK_UTIL_H