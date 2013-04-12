OpenCVLogger
============

	This is an adaptable Logger that will give output for many structures used in OpenCV, as well as some STL structures.

	- Compile using CMake

Usage
=====

	- namespace: nl_uu_science_gmt
 	- class name: Logger

	The logger is called by a macro defined in the header:
	CVLog(INFO) << "..." << variable << "," << structure;

	INFO goes to std::cout
	DEBUG goes to std::clog
	WARN goes to std::cerr
	ERROR goes to std::cerr

	The logger is capable of logging any primitive (unsigned char will be converted to int),
	a pointer will give the pointer address. std::pair, std::map, std::set,
	std::vector and std::deque give its contents

	And selected OpenCV data structures:
	- cv::Point eg.: (P;Q)
	- cv::Size eg.: w:P x h:Q
	- cv::Rect eg.: P,Q:WxH
	- cv::Range eg.: P<->Q (S)
	- cv::Mat eg.: Matlab-"like" representation
	- cv::Ptr<..> : dereferences the Ptr first

	Also logs to a log-file simultaneously by setting:
	- Logger::LogToFile
	- Logger::LogFileName

	- Silence logger: Logger::Quiet
	- Disable color : Logger::Color

