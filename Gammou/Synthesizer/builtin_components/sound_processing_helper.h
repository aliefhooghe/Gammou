#ifndef SOUND_PROCESSING_HELPER_H_
#define SOUND_PROCESSING_HELPER_H_


namespace Gammou {

	namespace Sound {


		template<class T>
		T constrain_in_range(const T value, const T min, const T max)
		{
			if (value < min)
				return min;
			else if (value > max)
				return max;
			else
				return value;
		}


	} /* Sound */

} /* Gammou*/


#endif
