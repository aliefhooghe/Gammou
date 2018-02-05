#ifndef STANDARD_SOUND_COMPONENT_H_
#define STANDARD_SOUND_COMPONENT_H_


#include "sound_component.h"


namespace Gammou {

	namespace Sound {


		/*
			Juste pour les essais, pas definitif
		*/

		class sin_oscilator : public sound_component {

		public:
			sin_oscilator(const unsigned int channel_count);
			~sin_oscilator() {};

			void process(const double input[]);
			void initialize_process();

		private:
			multi_channel_variable<double> m_freq_integral;

		};

		template<unsigned int Input_Count>
		class static_prod : public sound_component {

		public:
			static_prod(const unsigned int channel_count)
			: sound_component("StaticProd", Input_Count, 1, channel_count)
			{
				static_assert(Input_Count >= 2, "static_prod must have at least 2 input\n");
			}

			~static_prod() {};

			void process(const double input[])
			{
				double ret = input[0];
				for(unsigned int i = 1; i < Input_Count; ++i)
					ret *= input[i];
				m_output[0] = ret;
			}
		};

		template<unsigned int Input_Count>
		class static_sum : public sound_component {

		public:
			static_sum(const unsigned int channel_count)
			: sound_component("StaticSum", Input_Count, 1, channel_count)
			{
				static_assert(Input_Count >= 2, "static_sum must have at least 2 input\n");
			}

			~static_sum() {};

			void process(const double input[]) override
			{
				double ret = input[0];
				for(unsigned int i = 1; i < Input_Count; ++i)
					ret += input[i];
				m_output[0] = ret;
			}
		};


	} /* Sound */

} /* Gammou */




#endif /* STANDARD_SOUND_COMPONENT_H_ */
