#ifndef ABSTRACT_AUDIO_BACKEND_H_
#define ABSTRACT_AUDIO_BACKEND_H_

namespace Gammou {

	namespace AudioBackend {

	class abstract_audio_backend 
	{

	  public:
		virtual double get_parameter_value(
			const unsigned int index) = 0;

		virtual void set_parameter_value(
			const unsigned int index, const double value) = 0;

		virtual unsigned int get_parameter_count() = 0;
		};

	} /* AudioBackend */

} /* Gammou */

#endif // !ABSTRACT_AUDIO_BACKEND_H_
