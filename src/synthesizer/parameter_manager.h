#ifndef GAMMOU_PARAMETER_MANAGER_H_
#define GAMMOU_PARAMETER_MANAGER_H_

#include <array>
#include <algorithm>
#include <stack>

namespace Gammou {

    template <unsigned int ParameterCount>
    class parameter_manager {
        static constexpr auto default_shape_scale = 1.0f;
        static constexpr auto default_shape_base = 3.0f;
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
             *  \brief get the normalized parameter setting value.
             */
            float get_normalized() const noexcept
            {
                return _mgr.get_parameter_nomalized(_id);
            }

            /**
             *  \return a read only pointer to the smoothed value
             */
            const float* get_value_ptr() const noexcept
            {
                return _mgr.get_parameter_value_ptr(_id);
            }

            void set_shape_scale(float scale) noexcept
            {
                _mgr.set_parameter_shape_scale(_id, scale);
            }

            void set_shape_base(float base) noexcept
            {
                _mgr.set_parameter_shape_base(_id, base);
            }

            float get_shape_scale() const noexcept
            {
                return _mgr.get_parameter_shape_scale(_id);
            }

            float get_shape_base() const noexcept
            {
                return _mgr.get_parameter_shape_base(_id);
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
            std::fill_n(_parameter_normalized_settings.begin(), ParameterCount, 0.f);
            std::fill_n(_parameter_values.begin(), ParameterCount, 0.f);
            std::fill_n(_parameter_settings.begin(), ParameterCount, 0.f);
            std::fill_n(_shape_scales.begin(), ParameterCount, default_shape_scale);
            std::fill_n(_shape_bases.begin(), ParameterCount, default_shape_base);

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

        void set_parameter_shape_scale(param_id param, float scale) noexcept
        {
            _shape_scales[param] = scale;
            set_parameter_nomalized(param, _parameter_normalized_settings[param]);
        }

        void set_parameter_shape_base(param_id param, float base) noexcept
        {
            _shape_bases[param] = base;
            set_parameter_nomalized(param, _parameter_normalized_settings[param]);
        }

        float get_parameter_shape_scale(param_id param) const noexcept
        {
            return _shape_scales[param];
        }

        float get_parameter_shape_base(param_id param) const noexcept
        {
            return _shape_bases[param];
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

        parameter allocate_parameter(float initial_normalized_value = 0.f)
        {
            if (_free_params.empty()) {
                throw std::runtime_error("parameter_manager::allocate_parameter Not anymore free parameter to allocate !");
            }
            else {
                param_id new_id = _free_params.top();
                _free_params.pop();

                //  Set default shape and scales
                _shape_bases[new_id] = default_shape_base;
                _shape_scales[new_id] = default_shape_scale;

                //  Set the initial parameter value
                set_parameter_nomalized(new_id, initial_normalized_value);

                //  synchronize output value
                _parameter_values[new_id] = _parameter_settings[new_id];

                return parameter{*this, new_id};
            }
        }

        void set_parameter_nomalized(param_id param, float value) noexcept
        {
            const auto scale = _shape_scales[param];
            const auto base = _shape_bases[param];
            _parameter_normalized_settings[param] = value;
            _parameter_settings[param] = scale * (powf(base, value) - 1.f) / (base - 1.f);
        }


        float get_parameter_nomalized(param_id param) const noexcept
        {
            return _parameter_normalized_settings[param];
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
        std::array<float, ParameterCount> _parameter_normalized_settings;
        std::array<float, ParameterCount> _parameter_settings;
        std::array<float, ParameterCount> _parameter_values;
        std::array<float, ParameterCount> _shape_scales;
        std::array<float, ParameterCount> _shape_bases;
        float _dt;
        float _smooth_characteristic_time;
    };

}

#endif