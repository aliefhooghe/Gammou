#ifndef GAMMOU_PARAMETER_MANAGER_H_
#define GAMMOU_PARAMETER_MANAGER_H_

#include <array>
#include <algorithm>
#include <stack>

namespace Gammou {

    template <unsigned int ParameterCount>
    class parameter_manager {
    public:
        using param_id = unsigned int;
        class parameter {
            friend class parameter_manager;
        public:
            parameter(parameter&& other)
            :   _mgr{other._mgr}
            {
                _id = other._id;
                other._id = ParameterCount;
            }

            ~parameter() noexcept
            {
                if (_id != ParameterCount)
                    _mgr.free_parameter(_id);
            }

            /**
             *  \return the id identifying the parameter
             */
            param_id id() const noexcept
            {
                return _id;
            }

            /**
             *  \brief set the normalized parameter setting value.
             *  \param value the normalized value in [0., 1.]
             *  \details the actual parameter setting value will be computed
             *          according to the shape settings
             */
            void set_normalized(float value) noexcept
            {
                _mgr.set_parameter_nomalized(_id, value);
            }

            /**
             *  \return a read only pointer to the smoothed value
             */
            const float* get_value_ptr() const noexcept
            {
                return _mgr.get_parameter_value_ptr(_id);
            }


        private:
            parameter(parameter_manager& mgr, param_id id)
            : _mgr{mgr}, _id{id}
            {}

            parameter(const parameter&) = delete;
            auto& operator=(const parameter&) = delete;
            auto& operator=(parameter&&) = delete;

            parameter_manager& _mgr;
            param_id _id;
        };

        parameter_manager(float sample_rate, float smooth_characteristic_time = 0.1f) noexcept
        :   _dt{1.f / sample_rate},
            _smooth_characteristic_time{smooth_characteristic_time}
        {
            std::fill_n(_parameter_values.begin(), ParameterCount, 0.f);
            std::fill_n(_parameter_settings.begin(), ParameterCount, 0.f);

            //  At begining all parameter are free
            for (auto i = 0; i < ParameterCount; ++i)
                _free_params.push(i);
        }

        void set_sample_rate(float sample_rate) noexcept
        {
            _dt = 1.f / sample_rate;
        }

        void set_smooth_characteristic_time(float tau) noexcept
        {
            _smooth_characteristic_time = tau;
        }

        void process_one_sample() noexcept
        {
            const auto parameter_count = _parameter_values.size();
            const auto factor = _smooth_characteristic_time / _dt;
            const auto denominator = 1.f + factor;
            /*
             *  Each paramter is smoothed using a naive 1 pole low pass filter implementation.
             *  It is not required to use a more precise numerical method since parameter are changing
             *  at subsonic frequencies.
             */
            for (auto i = 0u; i < parameter_count; ++i) {
                _parameter_values[i] =
                    (_parameter_settings[i] + factor * _parameter_values[i]) /
                    denominator;
            }
        }

        parameter allocate_parameter(float initial_value = 0.f)
        {
            if (_free_params.empty()) {
                throw std::runtime_error("parameter_manager::allocate_parameter Not anymore free parameter to allocate !");
            }
            else {
                param_id new_id = _free_params.top();
                _free_params.pop();
                _parameter_values[new_id] = initial_value;
                _parameter_settings[new_id] = initial_value;
                return parameter{*this, new_id};
            }
        }

        void set_parameter_nomalized(param_id param, float value) noexcept
        {
            _parameter_settings[param] = value;
        }

        const float *get_parameter_value_ptr(param_id param) const noexcept
        {
            return &_parameter_values[param];
        }

        void free_parameter(param_id param) noexcept
        {
            _free_params.push(param);
        }

    private:
        std::stack<unsigned int> _free_params;
        std::array<float, ParameterCount> _parameter_settings;
        std::array<float, ParameterCount> _parameter_values;
        float _dt;
        float _smooth_characteristic_time;
    };

}

#endif