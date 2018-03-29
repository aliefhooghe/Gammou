
#include "sound_processing_helper.h"

namespace Gammou {

	namespace Sound {

		value_smoother::value_smoother(const double characteristic_time,
			const double initial_value)
			: m_characteristic_time(characteristic_time),
				m_current_value(initial_value)
		{
		}

		value_smoother::value_smoother(const double characteristic_time)
			: m_characteristic_time(characteristic_time),
			m_current_value(0.0)
		{
		}

		void value_smoother::set_characteristic_time(const double tau)
		{
			m_characteristic_time = tau;
		}

		void value_smoother::force_current_value(const double value)
		{
			m_current_value = value;
		}

		void value_smoother::tick(const double value_order, const double dt)
		{
			const double fact = m_characteristic_time / dt;
			m_current_value = (value_order + fact * m_current_value) / (1.0 + fact);
		}

		value_smoother::operator double()
		{
			return m_current_value;
		}

	} /* Sound */

} /* Gammou*/