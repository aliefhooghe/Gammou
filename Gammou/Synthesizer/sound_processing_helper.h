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

		class value_smoother {
		public:
			value_smoother(const double characteristic_time,
				const double initial_value);
			value_smoother(const double characteristic_time);

			void set_characteristic_time(const double tau);
			void force_current_value(const double value);
			void tick(const double value_order, const double dt);
			operator double();
		private:
			double m_characteristic_time;
			double m_current_value;
		};


	} /* Sound */

} /* Gammou*/


#endif
