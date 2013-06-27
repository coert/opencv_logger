/*
 * Logger.cpp
 *
 *  Created on: Mar 28, 2011
 *      Author: Coert van Gemeren (c.j.vangemeren@uu.nl)
 */
#include "Logger.h"

namespace nl_uu_science_gmt
{
bool Logger::Quiet = false;
bool Logger::Debug = false;
bool Logger::LogToFile = false;
bool Logger::Fixed = true;
bool Logger::Flush = false;
bool Logger::Color = false;

size_t Logger::Precision = 5;
size_t Logger::ReferenceWidth = 32;
size_t Logger::Size = 8;
Logger::LogFormat Logger::OutputFormat = Logger::FORMAT_DEFAULT;
std::string Logger::LogFileName = "log.txt";

const Logger::ImageTSMap Logger::ImageTypeStringMapping = Logger::initTypeStringMapping();
const Logger::ImageTSMap Logger::ImagePrimitiveStringMapping = Logger::initPrimitiveStringMapping();

const std::string Logger::Color_RED = "\033[1m\033[31m";
const std::string Logger::Color_GREEN = "\033[1m\033[32m";
const std::string Logger::Color_BLUE = "\033[1m\033[33m";
const std::string Logger::Color_YELLOW = "\033[1m\033[33m";
const std::string Logger::Color_CYAN = "\033[1m\033[36m";
const std::string Logger::Color_RESET = "\033[0m";

Logger Logger::create(const Logger::LogLevel level, const std::string file, const int line)
{
	Logger logger(level);

	std::stringstream __log_meta;
	__log_meta << getMicrotime();

	if (line > 0)
	{
		size_t __log_maxwidth = logger.getReferenceWidth();
		std::stringstream __log_msg_line;
		__log_msg_line << getStrippedFilename(file) << ":" << line;
		std::string __log_msg = __log_msg_line.str();
		size_t __log_length = MIN(__log_maxwidth, __log_msg.length());
		std::string __trimmed = __log_msg.substr(__log_msg.length() - __log_length, __log_length);
		std::string __log_pad;
		int __log_pad_length = MAX(__log_maxwidth - __trimmed.length(), 0);
		if (__log_pad_length > 0) __log_pad.insert(0, __log_pad_length, ' ');
		__log_meta << " " << __log_pad << __trimmed;
	}

	logger << __log_meta.str() << " " << getLevelDescr(level) << "\t";

	return logger;
}

const std::vector<std::pair<int, char*> > Logger::initTypeStringMapping()
{
	std::vector<std::pair<int, char*> > imageTypeStringMapping;
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8U, (char*) "CV_8U"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8UC2, (char*) "CV_8UC2"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8UC3, (char*) "CV_8UC3"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8UC4, (char*) "CV_8UC4"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8S, (char*) "CV_8S"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_16U, (char*) "CV_16U"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_16S, (char*) "CV_16S"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32S, (char*) "CV_32S"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32F, (char*) "CV_32F"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC2, (char*) "CV_32FC2"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC3, (char*) "CV_32FC3"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC4, (char*) "CV_32FC4"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC(5), (char*) "CV_32FC5"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64F, (char*) "CV_64F"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC2, (char*) "CV_64FC2"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC3, (char*) "CV_64FC3"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC4, (char*) "CV_64FC4"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC(5), (char*) "CV_64FC5"));

	return imageTypeStringMapping;
}

const std::vector<std::pair<int, char*> > Logger::initPrimitiveStringMapping()
{
	std::vector<std::pair<int, char*> > imageTypeStringMapping;
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8U, (char*) "uchar"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8UC2, (char*) "cv::Vec2b"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8UC3, (char*) "cv::Vec3b"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8UC4, (char*) "cv::Vec4b"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_8S, (char*) "char"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_16U, (char*) "ushort"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_16S, (char*) "short"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32S, (char*) "int"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32F, (char*) "float"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC2, (char*) "cv::Vec2f"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC3, (char*) "cv::Vec3f"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC4, (char*) "cv::Vec4f"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_32FC(5), (char*) "cv::Vec<float, 5>"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64F, (char*) "double"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC2, (char*) "cv::Vec2d"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC3, (char*) "cv::Vec3d"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC4, (char*) "cv::Vec4d"));
	imageTypeStringMapping.push_back(std::pair<int, char*>(CV_64FC(5), (char*) "cv::Vec<double, 5>"));

	return imageTypeStringMapping;
}

const char* Logger::getMatDepthFromCode(int depth)
{
	for (size_t i = 0; i < ImageTypeStringMapping.size(); ++i)
	{
		if (ImageTypeStringMapping[i].first == depth)
		{
			return ImageTypeStringMapping[i].second;
		}
	}
	return "CV_???";
}

const char* Logger::getMatPrimitiveFromCode(int depth)
{
	for (size_t i = 0; i < ImagePrimitiveStringMapping.size(); ++i)
	{
		if (ImagePrimitiveStringMapping[i].first == depth)
		{
			return ImagePrimitiveStringMapping[i].second;
		}
	}
	return "???";
}

std::string Logger::getDatestamp(time_t unix_t)
{
	time_t in_time;

	if (unix_t == 0)
		in_time = time(NULL);
	else
		in_time = unix_t;

	tm tm_buf;
	struct tm* t = localtime_r(&in_time, &tm_buf);

	std::stringstream date_day;
	date_day << t->tm_mday;
	std::string d = date_day.str();
	if (d.length() < 2) d = "0" + d;

	std::stringstream date_m;
	date_m << (1 + t->tm_mon);
	std::string m = date_m.str();
	if (m.length() < 2) m = "0" + m;

	std::stringstream date_y;
	date_y << (1900 + t->tm_year);
	std::string y = date_y.str();

	std::stringstream date_string;
	date_string << d << "-" << m << "-" << y;

	return date_string.str();
}

std::string Logger::getTimestamp(time_t unix_t)
{
	time_t in_time;

	if (unix_t == 0)
	{
		in_time = time(NULL);
	}
	else
	{
		in_time = unix_t;
	}

	tm tm_buf;
	struct tm* t = localtime_r(&in_time, &tm_buf);

	std::stringstream time_hr;
	time_hr << t->tm_hour;
	std::string hr = time_hr.str();
	if (hr.length() < 2) hr = "0" + hr;

	std::stringstream time_mnt;
	time_mnt << t->tm_min;
	std::string mnt = time_mnt.str();
	if (mnt.length() < 2) mnt = "0" + mnt;

	std::stringstream time_sec;
	time_sec << t->tm_sec;
	std::string sec = time_sec.str();
	if (sec.length() < 2) sec = "0" + sec;

	std::stringstream date_string;
	date_string << getDatestamp() << " " << hr << ":" << mnt << ":" << sec;

	return date_string.str();
}

std::string Logger::getMicrotime(time_t unix_t)
{
	time_t in_time;

	if (unix_t == 0)
	{
		in_time = time(NULL);
	}
	else
	{
		in_time = unix_t;
	}

	tm tm_buf;
	struct tm* t = localtime_r(&in_time, &tm_buf);

	std::stringstream time_hr;
	time_hr << t->tm_hour;
	std::string hr = time_hr.str();
	if (hr.length() < 2) hr = "0" + hr;

	std::stringstream time_mnt;
	time_mnt << t->tm_min;
	std::string mnt = time_mnt.str();
	if (mnt.length() < 2) mnt = "0" + mnt;

	std::stringstream time_sec;
	time_sec << t->tm_sec;
	std::string sec = time_sec.str();
	if (sec.length() < 2) sec = "0" + sec;

	timeval microtime;
	gettimeofday(&microtime, NULL);
	int milli = microtime.tv_usec / 1000;

	std::stringstream time_msec;
	time_msec << milli;
	std::string msec = time_msec.str();
	if (msec.length() < 2)
		msec = "00" + msec;
	else if (msec.length() < 3) msec = "0" + msec;

	std::stringstream date_string;
	date_string << hr << ":" << mnt << ":" << sec << "." << msec;

	return date_string.str();
}

std::string Logger::getStrippedFilename(const std::string &filename)
{
	std::string output = filename;
	replaceAll(output, ".cpp", "");

	return output;
}

std::string Logger::getLevelDescr(LogLevel level)
{
	switch (level)
	{
		case LOG_INFO:
			return "INFO ";
		case LOG_DEBUG:
			return "DEBUG";
		case LOG_WARN:
			return "WARN ";
		case LOG_ERROR:
			return "ERROR";
		default:
			return "INFO ";
	}
}

void Logger::replaceAll(std::string &source, const std::string &find, const std::string &replace)
{
	size_t start_pos = 0;
	while ((start_pos = source.find(find, start_pos)) != std::string::npos)
	{
		source.replace(start_pos, find.length(), replace);
		start_pos += replace.length();
	}
}

//bool Logger::openLogfile(const std::string &log_file)
//{
//	_log_to_file = true;
//
//	if (!_stream->file_buffer.is_open())
//	{
//		_log_file_name = log_file;
//		_stream->file_buffer.open(_log_file_name.c_str(), std::ofstream::app);
//		_stream->file_buffer << "-----------============ start: " << getTimestamp() << " ============-----------\n";
//	}
//
//	return _stream->file_buffer.is_open();
//}

//bool Logger::closeLogfile()
//{
//	if (_stream->file_buffer.is_open())
//	{
//		_stream->file_buffer << "-----------============ stop:  " << getTimestamp() << " ============-----------\n";
//		_stream->file_buffer.flush();
//		_stream->file_buffer.close();
//	}
//
//	_log_to_file = false;
//
//	return _stream->file_buffer.is_open();
//}

void Logger::output()
{
	const std::string input = _stream->buffer.str();

	if (_stream->log_level > LOG_WARN)
	{
		if (_color) std::cerr << Color_RED;
		std::cerr << input << std::endl;
		if (_color) std::cerr << Color_RESET;
	}
	else if (_stream->log_level == LOG_WARN)
	{
		if (_color) std::cerr << Color_YELLOW;
		std::cerr << input << std::endl;
		if (_color) std::cerr << Color_RESET;
	}
	else if (_stream->log_level == LOG_DEBUG && (_debug || !_quiet))
	{
		if (_color) std::clog << Color_CYAN;
		std::clog << input << std::endl;
		if (_color) std::clog << Color_RESET;
	}
	else if (_stream->log_level == LOG_INFO && !_quiet)
	{
		std::cout << input << std::endl;
		if (_color) std::cout << Color_RESET;
	}
}

void Logger::write()
{
	const std::string input = _stream->buffer.str();

	if (_stream->file_buffer.is_open())
	{
		_stream->file_buffer << input << std::endl;
	}
//		else if (openLogfile(_log_file_name))
//		{
//			_stream->file_buffer << input;
//		}
	else
	{
		if (_color) std::cerr << Color_RED;
		std::cerr << "Unable to open logfile: " << _log_file_name << std::endl;
		if (_color) std::cerr << Color_RESET;
	}
//	else if (_stream->file_buffer.is_open())
//	{
//		closeLogfile();
//	}

	if (_flush) _stream->file_buffer.flush();
}

Logger& Logger::operator<<(const char* input)
{
	_stream->buffer << input;
	return *this;
}

/*
 * Cast unsigned char to int to show it as a number
 */
Logger& Logger::operator<<(uchar input)
{
	*this << (int) input;
	return *this;
}

Logger& Logger::operator<<(short input)
{
	doIntegerInputMarkup(input, "%d");
	return *this;
}

Logger& Logger::operator<<(ushort input)
{
	doIntegerInputMarkup(input, "%u");
	return *this;
}

Logger& Logger::operator<<(int input)
{
	doIntegerInputMarkup(input, "%d");
	return *this;
}

Logger& Logger::operator<<(long unsigned int input)
{
	doIntegerInputMarkup(input, "%lu");
	return *this;
}

Logger& Logger::operator<<(long input)
{
	doIntegerInputMarkup(input, "%ld");
	return *this;
}

Logger& Logger::operator<<(float input)
{
	doRealInputMarkup(input, "f");
	return *this;
}

Logger& Logger::operator<<(double input)
{
	doRealInputMarkup(input, "f");
	return *this;
}

Logger& Logger::operator<<(bool input)
{
	if (input)
		*this << "true";
	else
		*this << "false";
	return *this;
}

Logger& Logger::operator<<(const cv::Point& input)
{
	*this << "(" << input.x << ";" << input.y << ")";
	return *this;
}

Logger& Logger::operator<<(const cv::Size& input)
{
	*this << "w:" << input.width << " x h:" << input.height;
	return *this;
}

Logger& Logger::operator<<(const cv::Scalar& input)
{
	for (int i = 0; i < input.channels; i++)
		*this << input[i] << ",";
	return *this;
}

Logger& Logger::operator<<(const cv::Rect& input)
{
	*this << input.x << "," << input.y << ":" << input.width << "x" << input.height;
	return *this;
}

Logger& Logger::operator<<(const cv::Range& range)
{
	bool s = _singular;
	_singular = true;
	*this << range.start << "<->" << range.end << " (" << range.size() << ")";
	_singular = s;
	return *this;
}

Logger& Logger::operator<<(const cv::Mat& mat)
{
	bool s = _singular;
	_singular = false;
	_matrix_type = mat.type();

	switch (_matrix_type)
	{
		case CV_8U:
		{
			cv::Mat_<uchar> type = mat;
			*this << type;
			break;
		}
		case CV_8UC2:
		{
			cv::Mat_<cv::Vec2b> type = mat;
			*this << type;
			break;
		}
		case CV_8UC3:
		{
			cv::Mat_<cv::Vec3b> type = mat;
			*this << type;
			break;
		}
		case CV_8UC4:
		{
			cv::Mat_<cv::Vec4b> type = mat;
			*this << type;
			break;
		}
		case CV_16U:
		{
			cv::Mat_<ushort> type = mat;
			*this << type;
			break;
		}
		case CV_16S:
		{
			cv::Mat_<short> type = mat;
			*this << type;
			break;
		}
		case CV_32S:
		{
			cv::Mat_<int> type = mat;
			*this << type;
			break;
		}
		case CV_32SC2:
		{
			cv::Mat_<cv::Vec2i> type = mat;
			*this << type;
			break;
		}
		case CV_32SC3:
		{
			cv::Mat_<cv::Vec3i> type = mat;
			*this << type;
			break;
		}
		case CV_32SC4:
		{
			cv::Mat_<cv::Vec4i> type = mat;
			*this << type;
			break;
		}
		case CV_32F:
		{
			cv::Mat_<float> type = mat;
			*this << type;
			break;
		}
		case CV_32FC2:
		{
			cv::Mat_<cv::Vec2f> type = mat;
			*this << type;
			break;
		}
		case CV_32FC3:
		{
			cv::Mat_<cv::Vec3f> type = mat;
			*this << type;
			break;
		}
		case CV_32FC4:
		{
			cv::Mat_<cv::Vec4f> type = mat;
			*this << type;
			break;
		}
		case CV_64F:
		{
			cv::Mat_<double> type = mat;
			*this << type;
			break;
		}
		case CV_64FC2:
		{
			cv::Mat_<cv::Vec2d> type = mat;
			*this << type;
			break;
		}
		case CV_64FC3:
		{
			cv::Mat_<cv::Vec3d> type = mat;
			*this << type;
			break;
		}
		case CV_64FC4:
		{
			cv::Mat_<cv::Vec4d> type = mat;
			*this << type;
			break;
		}
	}

	_singular = s;
	return *this;
}

} /* namespace nl_uu_science_gmt */
