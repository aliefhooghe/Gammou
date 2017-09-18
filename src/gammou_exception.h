#ifndef GAMMOU_EXCEPTION_H_
#define GAMMOU_EXCEPTION_H_


#include <exception>


namespace Gammou {

class error : public std::exception{

public:
	virtual const char *what() const throw ()
	{
		return "Gammou generic error";
	}
};


} /* namespace Gammou */

#endif /* GAMMOU_EXCEPTION_H_ */











