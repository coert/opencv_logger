/*
 * Logger.h
 *
 *  Created on: Mar 28, 2011
 *      Author: Coert van Gemeren (c.j.vangemeren@uu.nl)
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/time.h>

#include <deque>
#include <map>
#include <set>
#include <vector>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "opencv2/core/core.hpp"

#define CVLog(level) nl_uu_science_gmt::Logger::create(nl_uu_science_gmt::Logger::LOG_##level, __FILE__, __LINE__)

namespace nl_uu_science_gmt
{

class Logger
{
public:
	enum LogLevel
	{
		LOG_INFO, LOG_DEBUG, LOG_WARN, LOG_ERROR
	};
	enum LogFormat
	{
		FORMAT_DEFAULT, FORMAT_MATLAB, FORMAT_CSV, FORMAT_C, FORMAT_OPENCV
	};

	static bool Quiet;
	static bool Debug;
	static bool LogToFile;
	static bool Fixed;
	static bool Flush;
	static bool Color;

	static size_t Precision;
	static size_t ReferenceWidth;
	static size_t Size;
	static LogFormat OutputFormat;
	static LogLevel Level;
	static std::string LogFileName;

	const static std::string Color_RED;
	const static std::string Color_GREEN;
	const static std::string Color_BLUE;
	const static std::string Color_YELLOW;
	const static std::string Color_CYAN;
	const static std::string Color_RESET;

private:
	struct Stream
	{
		Stream(LogLevel l) :
				log_level(l), log_filename("")
		{
		}
		Stream(LogLevel l, const std::string &f) :
				log_level(l), log_filename(f)
		{
		}

		const LogLevel log_level;
		const std::string &log_filename;
		std::ofstream file_buffer;

		std::stringstream buffer;

	}*_stream;

	const LogFormat _output_format;

	const bool _quiet;
	const bool _debug;
	const bool _fixed;
	const bool _flush;
	const bool _color;

	const size_t _precision;
	const size_t _reference_width;

	size_t _size;

	bool _log_to_file;
	std::string _log_file_name;

	std::vector<size_t> _channel_widths;
	bool _singular;
	int _matrix_type;
	int _dimension;
	int _dimensions;

	typedef std::vector<std::pair<int, char*> > ImageTSMap;
	static const ImageTSMap ImageTypeStringMapping;
	static const ImageTSMap ImagePrimitiveStringMapping;
	static const std::vector<std::pair<int, char*> > initTypeStringMapping();
	static const std::vector<std::pair<int, char*> > initPrimitiveStringMapping();
	static const char* getMatDepthFromCode(int);
	static const char* getMatPrimitiveFromCode(int);

	static std::string getDatestamp(time_t unix_t = 0);
	static std::string getTimestamp(time_t unix_t = 0);

	static inline void replaceAll(std::string &, const std::string &, const std::string &);

	template<typename T>
	inline void doIntegerInputMarkup(const T &input, const std::string &type)
	{
		if (!_singular)
		{
			char buffer[11];
			sprintf(buffer, type.c_str(), input);

			std::string space;
			if (_size > 0) space = std::string(_size - std::string(buffer).length(), ' ');

			*this << space << buffer;
		}
		else
		{
			char buffer[11];
			sprintf(buffer, type.c_str(), input);

			*this << buffer;
		}
	}

	template<typename T>
	inline void doRealInputMarkup(const T &input, const std::string &type)
	{
		if (!_singular)
		{
			if (input != 0.f)
			{
				*this << getRealInputMarkUp(input, "f");
			}
			else
			{
				std::string space(_size - 1, ' ');
				*this << space << "0";
			}
		}
		else
		{
			size_t size = _size;
			_size = 0;
			*this << getRealInputMarkUp(input, "f");
			_size = size;
		}
	}

	template<typename T>
	inline std::string getRealInputMarkUp(const T &input, const std::string &type)
	{
		char buffer[256];
		std::stringstream layout;
		layout << "%" << (int) _size << "." << (int) _precision << type;
		sprintf(buffer, layout.str().c_str(), input);
		return buffer;
	}

	std::string getDigitWidth(double mVal)
	{
		std::stringstream layout, ssize;
		layout << "%0." << (int) _precision << "f";

		if (mVal != 0.f)
		{
			char buffer[256];
			sprintf(buffer, layout.str().c_str(), mVal);

			ssize << buffer;
		}
		else
		{
			ssize << "0";
		}

		return ssize.str();
	}

	template<typename T, int c>
	std::vector<size_t> getValueWidth(const cv::Vec<T, c> &v, const cv::Mat &m)
	{
		std::vector<size_t> widths;

		for (int i = 0; i < m.channels(); ++i)
		{
			T value = 0;
			cv::Mat channel_vals;
			for (int r = 0; r < m.rows; ++r)
			{
				cv::Vec<T, c> v = m.at<cv::Vec<T, c> >(0, r);
				value = v[i];
				channel_vals.push_back(value);
			}

			std::vector<size_t> width = getValueWidth(value, channel_vals);

			widths.push_back(width.front());
		}

		return widths;
	}

	std::vector<size_t> getValueWidth(const double &value, const cv::Mat &m)
	{
		double minVal = INFINITY, maxVal = -INFINITY;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::string ssize_min, ssize_max;
		ssize_min = getDigitWidth(minVal);
		ssize_max = getDigitWidth(maxVal);

		return std::vector<size_t>(1, MAX(ssize_min.length(), ssize_max.length()));
	}

	std::vector<size_t> getValueWidth(const float &value, const cv::Mat &m)
	{
		double minVal = INFINITY, maxVal = -INFINITY;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::string ssize_min, ssize_max;
		ssize_min = getDigitWidth(minVal);
		ssize_max = getDigitWidth(maxVal);

		return std::vector<size_t>(1, MAX(ssize_min.length(), ssize_max.length()));
	}

	std::vector<size_t> getValueWidth(const uchar &value, const cv::Mat &m)
	{
		double minVal = INFINITY, maxVal = -INFINITY;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::stringstream ssize_min, ssize_max;
		ssize_min << (int) minVal;
		ssize_max << (int) maxVal;

		return std::vector<size_t>(1, MAX(ssize_min.str().length(), ssize_max.str().length()));
	}

	template<typename T>
	std::vector<size_t> getValueWidth(const T &value, const cv::Mat &m)
	{
		double minVal = INFINITY, maxVal = -INFINITY;
		cv::minMaxIdx(m, &minVal, &maxVal);

		std::stringstream ssize_min, ssize_max;
		ssize_min << (T) minVal;
		ssize_max << (T) maxVal;

		return std::vector<size_t>(1, MAX(ssize_min.str().length(), ssize_max.str().length()));
	}

public:
	inline Logger(LogLevel l, const std::string f = LogFileName) :
			_stream(new Stream(l, f)), _output_format(OutputFormat), _quiet(Quiet), _debug(Debug), _fixed(Fixed), _flush(
					Flush), _color(Color), _precision(Precision), _reference_width(ReferenceWidth), _size(Size), _log_to_file(
					LogToFile), _log_file_name(LogFileName), _singular(true), _matrix_type(0), _dimension(0), _dimensions(0)
	{
		if (isLogToFile())
		{
			_stream->file_buffer.open(_log_file_name.c_str(), std::ofstream::app);
			if (!_stream->file_buffer.is_open())
			{
				std::string path = _log_file_name;
				path = path.substr(0, path.find_last_of("\\/"));
				boost::filesystem::create_directories(path);
				_stream->file_buffer.open(_log_file_name.c_str(), std::ofstream::app);
				assert(_stream->file_buffer.is_open());
			}
		}
	}

	inline ~Logger()
	{
		output();

		if (isLogToFile())
		{
			write();
			_stream->file_buffer.close();
		}
		delete _stream;
	}

	static Logger create(const Logger::LogLevel, const std::string = "", const int = 0);

	static std::string getMicrotime(time_t unix_t = 0);
	static std::string getStrippedFilename(const std::string &);
	static std::string getLevelDescr(LogLevel);

	template<typename T>
	Logger& operator<<(const T& input)
	{
		std::stringstream buffer;
		buffer << input;
		*this << buffer.str().c_str();
		return *this;
	}

	template<typename T, typename U>
	Logger& operator<<(const std::pair<T, U>& input)
	{
		_singular = true;
		*this << input.first << ":" << input.second;
		return *this;
	}

	template<typename T, typename U>
	Logger& operator<<(const std::map<T, U>& input)
	{
		_singular = true;
		typename std::map<T, U>::const_iterator pair = input.begin();
		*this << "[";
		for (size_t i = 0; i < input.size(); i++, ++pair)
			*this << *pair << ", ";
		*this << "]";

		return *this;
	}

	template<typename T>
	Logger& operator<<(const std::set<T>& input)
	{
		_singular = true;
		typename std::set<T>::const_iterator iter = input.begin();
		*this << "(" << input.size() << "):";

		for (size_t i = 0; i < input.size(); i++, ++iter)
			*this << *iter << ", ";

		return *this;
	}

	template<typename T>
	Logger& operator<<(const std::vector<T>& input)
	{
		_singular = true;
		typename std::vector<T>::const_iterator iter = input.begin();
		*this << "(" << input.size() << "):";

		for (size_t i = 0; i < input.size(); i++, ++iter)
			*this << *iter << ", ";

		return *this;
	}

	template<typename T>
	Logger& operator<<(const std::deque<T>& input)
	{
		_singular = true;
		typename std::deque<T>::const_iterator iter = input.begin();
		*this << "(" << input.size() << "):";

		for (size_t i = 0; i < input.size(); i++, ++iter)
			*this << *iter << ",";

		return *this;
	}

	template<typename T>
	Logger& operator<<(cv::Mat_<T>& matrix)
	{
		cv::Mat t_m = matrix;

		if (t_m.dims < 3)
		{
			// display 2D matrix

			size_t s = _size;
			_size = 0;

			switch (_output_format)
			{
				case FORMAT_OPENCV:
					*this << "cv::Mat var = (cv::Mat_<" << getMatPrimitiveFromCode(_matrix_type) << " >";
					*this << "(" << t_m.rows << ", " << t_m.cols << ") << \\\n";
					break;
				case FORMAT_MATLAB:
				case FORMAT_CSV:
					break;
				case FORMAT_DEFAULT:
				default:
					*this << getMatDepthFromCode(matrix.type()) << "(" << t_m.rows << "x" << t_m.cols << ")\n";
					break;
			}

			_size = s;

			std::vector<std::vector<size_t> > size;

			for (int y = 0; y < t_m.rows; y++)
			{
				switch (_output_format)
				{
					case FORMAT_MATLAB:
					{
						if (y > 0)
							*this << "  ";
						else
							*this << "  ";
						break;
					}
					case FORMAT_C:
					{
						if (y > 0)
							*this << "  ";
						else
							*this << "{ ";
						break;
					}
					case FORMAT_CSV:
						break;
					case FORMAT_DEFAULT:
					default:
						break;
				}

				for (int x = 0; x < t_m.cols; x++)
				{
					if (y == 0)
					{
						cv::Mat row = t_m.col(x).clone();
						T& wp = matrix(cv::Point(x, y));
						size.push_back(getValueWidth(wp, row));
					}

					_channel_widths = size.at(x);
					_size = _channel_widths.front();
					T& point = matrix(cv::Point(x, y));

					switch (_output_format)
					{
						case FORMAT_OPENCV:
						{
							if (y == t_m.rows - 1 && x == t_m.cols - 1)
								*this << point;
							else if (x == 0)
								*this << "               " << point << ", ";
							else if (x == t_m.cols - 1)
								*this << point << ", \\";
							else
								*this << point << ", ";
							break;
						}
						case FORMAT_MATLAB:
						{
							if (x == t_m.cols - 1)
								*this << point;
							else
								*this << point << " ";
							break;
						}
						case FORMAT_C:
						{
							if (y == t_m.rows - 1 && x == t_m.cols - 1)
								*this << point << " ";
							else if (x == t_m.cols - 1)
								*this << point << ", \\";
							else
								*this << point << ", ";
							break;
						}
						case FORMAT_CSV:
						{
							if (x == t_m.cols - 1)
								*this << point;
							else
								*this << point << ", ";
							break;
						}
						case FORMAT_DEFAULT:
						default:
							*this << point << " ";
							break;
					}
				}

				switch (_output_format)
				{
					case FORMAT_OPENCV:
					{
						if (y != t_m.rows - 1) *this << "\n";
						break;
					}
					case FORMAT_MATLAB:
					{
						if (y != t_m.rows - 1) *this << ";\n";
						break;
					}
					case FORMAT_C:
					{
						if (y != t_m.rows - 1) *this << "\n";
						break;
					}
					case FORMAT_DEFAULT:
					default:
						*this << "\n";
						break;
				}
			}

			switch (_output_format)
			{
				case FORMAT_OPENCV:
					*this << ");\n";
					break;
				case FORMAT_MATLAB:
				{
					if (_dimension != _dimensions - 1)
						*this << ",\n";
					else
						*this << "]";
					break;
				}
				case FORMAT_C:
					*this << "}\n";
					break;
				case FORMAT_DEFAULT:
				default:
					break;
			}

			_size = s;
		}
		else
		{
			// handle high dimensional matrices

			size_t s = _size;
			_size = 0;
			for (int d = 0; d < t_m.dims; ++d)
			{
				*this << t_m.size[d];
				if (d + 1 == t_m.dims)
					*this << "\n";
				else
					*this << "x";
			}
			switch (_output_format)
			{
				case FORMAT_MATLAB:
					*this << "reshape([ ";
					break;
				default:
					break;
			}
			_size = s;

			int d = 0;
			_dimensions = t_m.size[d];
			for (int c = 0; c < t_m.size[d]; ++c)
			{
				_dimension = c;
				size_t s = _size;
				_size = 0;
				switch (_output_format)
				{
					case FORMAT_OPENCV:
						break;
					case FORMAT_MATLAB:
						break;
					case FORMAT_CSV:
						break;
					case FORMAT_DEFAULT:
					default:
						*this << c << ", ";
						break;
				}

				_size = s;

				std::vector<cv::Range> range;
				for (int rd = 0; rd < t_m.dims; ++rd)
				{
					if (rd == d)
						range.push_back(cv::Range(c, c + 1));
					else
						range.push_back(cv::Range::all());
				}

				cv::Mat p = t_m(&range[0]).clone();
				std::vector<int> sizedmin;
				for (int dmin = d + 1; dmin < t_m.dims; ++dmin)
					sizedmin.push_back(t_m.size[dmin]);

				cv::Mat pdmin;
				pdmin.create(t_m.dims - 1, &sizedmin[0], p.type());
				pdmin.data = p.data;

				*this << pdmin;

				switch (_output_format)
				{
					default:
						if (c != t_m.size[d] - 1 && d != t_m.dims - 1) *this << "\n";
						break;
				}
			}

			_size = 0;
			switch (_output_format)
			{
				case FORMAT_MATLAB:
					*this << ",[";
					for (int d = 0; d < t_m.dims; ++d)
						*this << t_m.size[d] << " ";
					*this << "])";
					break;
				default:
					break;
			}
			_size = s;
		}

		return *this;
	}

	template<typename T>
	Logger& operator<<(const cv::Ptr<T>& p_mat)
	{
		*this << *p_mat;
		return *this;
	}

	template<typename T, int c>
	Logger& operator<<(cv::Vec<T, c>& vector)
	{
		T* val = vector.val;
		switch (_output_format)
		{
			case FORMAT_OPENCV:
				*this << getMatPrimitiveFromCode(_matrix_type) << "(";
				break;
			case FORMAT_C:
			case FORMAT_CSV:
			case FORMAT_MATLAB:
				break;
			default:
			case FORMAT_DEFAULT:
				*this << "(";
				break;
		}
		for (int i = 0; i < c; i++)
		{
			_size = _channel_widths.at(i);
			switch (_output_format)
			{
				case FORMAT_MATLAB:
					*this << val[i] << " ";
					break;
				case FORMAT_OPENCV:
				case FORMAT_C:
				case FORMAT_CSV:
					*this << val[i];
					if (i < c - 1) *this << ",";
					break;
				default:
				case FORMAT_DEFAULT:
					*this << val[i];
					if (i < c - 1) *this << ";";
					break;
			}
		}
		switch (_output_format)
		{
			case FORMAT_C:
			case FORMAT_CSV:
			case FORMAT_MATLAB:
				break;
			case FORMAT_OPENCV:
			case FORMAT_DEFAULT:
			default:
				*this << ")";
				break;
		}

		return *this;
	}

	Logger& operator<<(const char*);
	Logger& operator<<(uchar);
	Logger& operator<<(double);
	Logger& operator<<(float);
	Logger& operator<<(long);
	Logger& operator<<(int);
	Logger& operator<<(short);
	Logger& operator<<(ushort);
	Logger& operator<<(bool);
	Logger& operator<<(long unsigned int);

	Logger& operator<<(const cv::Mat&);
	Logger& operator<<(const cv::Size&);
	Logger& operator<<(const cv::Scalar&);
	Logger& operator<<(const cv::Point&);
	Logger& operator<<(const cv::Rect&);
	Logger& operator<<(const cv::Range&);

	void output();
	void write();

	bool isFixed() const
	{
		return _fixed;
	}

	bool isFlush() const
	{
		return _flush;
	}

	const std::string& getLogFileName() const
	{
		return _log_file_name;
	}

	bool isLogToFile() const
	{
		return _log_to_file;
	}

	LogFormat getOutputFormat() const
	{
		return _output_format;
	}

	size_t getPrecision() const
	{
		return _precision;
	}

	bool isQuiet() const
	{
		return _quiet;
	}

	size_t getReferenceWidth() const
	{
		return _reference_width;
	}

	size_t getSize() const
	{
		return _size;
	}

	bool isColor() const
	{
		return _color;
	}
};

} /* namespace nl_uu_science_gmt */
#endif /* LOGGER_H_ */
