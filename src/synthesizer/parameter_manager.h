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

            param_id id() const noexcept
            {
                return _id;
            }

            void set(float value)
            {
                _mgr.set_parameter(_id, value);
            }

            const float* get_value_ref()
            {
                return _mgr.get_parameter_value_ref(_id);
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

        parameter_manager() noexcept
        {
            std::fill_n(_parameter_values.begin(), ParameterCount, 0.f);
            std::fill_n(_parameter_settings.begin(), ParameterCount, 0.f);

            //  At begining all parameter are free
            for (auto i = 0; i < ParameterCount; ++i)
                _free_params.push(i);
        }

        void process_one_sample() noexcept
        {
            const auto parameter_count = _parameter_values.size();
            /** @todo smooth using 1-pole low pass filter **/
            for (auto i = 0u; i < parameter_count; ++i)
                _parameter_values[i] = _parameter_settings[i];
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

        void set_parameter(param_id param, float value) noexcept
        {
            _parameter_settings[param] = value;
        }

        const float *get_parameter_value_ref(param_id param) const noexcept
        {
            if (param < ParameterCount)
                return &_parameter_values[param];
            else
                return nullptr;
        }

        void free_parameter(param_id param) noexcept
        {
            if (param < ParameterCount)
                _free_params.push(param);
        }

    private:
        std::stack<unsigned int> _free_params;
        std::array<float, ParameterCount> _parameter_settings;
        std::array<float, ParameterCount> _parameter_values;
    };

}

#endif