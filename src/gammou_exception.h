#ifndef GAMMOU_EXCEPTION_H_
#define GAMMOU_EXCEPTION_H_


#include <iostream>
#include <exception>




namespace process {

class error : public std::exception{

public:
	virtual const char *what() const throw ()
	{
		return "Gammou generic error";
	}
};




} /* namespace Gammou */

/*
 * 	For debug
 */

#ifdef NDEBUG
#define DEBUG_PRINT(...)
#else
#define DEBUG_PRINT(...) std::fprintf(stderr, __VA_ARGS__)
#endif





#endif /* GAMMOU_EXCEPTION_H_ */











