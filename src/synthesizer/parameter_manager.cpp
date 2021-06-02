
#include <cmath>
#include <stdexcept>

#include "parameter_manager.h"

namespace Gammou
{

    parameter_manager::parameter_manager(float sample_rate, float smooth_characteristic_time) noexcept
        : _dt{1.f / sample_rate},
          _smooth_characteristic_time{smooth_characteristic_time}
    {
    }

    void parameter_manager::set_sample_rate(float sample_rate) noexcept
    {
        _dt = 1.f / sample_rate;
    }

    void parameter_manager::set_smooth_characteristic_time(float tau) noexcept
    {
        _smooth_characteristic_time = tau;
    }

    void parameter_manager::set_parameter_shape_scale(param_id param, float scale) noexcept
    {
        _shape_scales[param] = scale;
        set_parameter_nomalized(param, _parameter_normalized_settings[param]);
    }

    void parameter_manager::set_parameter_shape_base(param_id param, float base) noexcept
    {
        _shape_bases[param] = base;
        set_parameter_nomalized(param, _parameter_normalized_settings[param]);
    }

    float parameter_manager::get_parameter_shape_scale(param_id param) const noexcept
    {
        return _shape_scales[param];
    }

    float parameter_manager::get_parameter_shape_base(param_id param) const noexcept
    {
        return _shape_bases[param];
    }

    void parameter_manager::process_one_sample() noexcept
    {
        const auto parameter_count = _parameter_values.size();
        const auto factor = _smooth_characteristic_time / _dt;
        const auto denominator = 1.f + factor;
        /*
         *  Each paramter is smoothed using a naive 1 pole low pass filter implementation.
         *  It is not required to use a more precise numerical method since parameter are changing
         *  at subsonic frequencies.
         */
        for (auto i = 0u; i < parameter_count; ++i)
        {
            _parameter_values[i] =
                (_parameter_settings[i] + factor * _parameter_values[i]) /
                denominator;
        }
    }

    parameter_manager::parameter parameter_manager::allocate_parameter(float initial_normalized_value)
    {
        param_id new_id = INVALID_PARAM;

        if (_free_params.empty())
        {
            new_id = _parameter_values.size();

            //  Allocate memory and Set default shape and scales
            _shape_scales.push_back(default_shape_scale);
            _shape_bases.push_back(default_shape_base);
            _parameter_normalized_settings.push_back(0.f);
            _parameter_values.push_back(0.f);
            _parameter_settings.push_back(0.f);
            _callbacks.emplace_back(control_changed_callback{});
        }
        else
        {
            new_id = _free_params.top();
            _free_params.pop();

            //  Set default shape and scales
            _shape_bases[new_id] = default_shape_base;
            _shape_scales[new_id] = default_shape_scale;
        }

        //  Set the initial parameter value
        set_parameter_nomalized(new_id, initial_normalized_value);

        //  synchronize output value
        _parameter_values[new_id] = _parameter_settings[new_id];

        return parameter{*this, new_id};
    }

    void parameter_manager::set_parameter_nomalized(param_id param, float value) noexcept
    {
        const auto scale = _shape_scales[param];
        const auto base = _shape_bases[param];
        _parameter_normalized_settings[param] = value;
        _parameter_settings[param] = scale * (powf(base, value) - 1.f) / (base - 1.f);
        
        if (auto& callback = _callbacks[param])
            callback();
    }

    float parameter_manager::get_parameter_nomalized(param_id param) const noexcept
    {
        return _parameter_normalized_settings[param];
    }

    const float *parameter_manager::get_parameter_value_ptr(param_id param) const noexcept
    {
        return &_parameter_values[param];
    }

    void parameter_manager::set_control_changed_callback(param_id param, control_changed_callback callback) noexcept
    {
        _callbacks[param] = callback;
    }

    void parameter_manager::_free_parameter(param_id param) noexcept
    {
        _free_params.push(param);
        _callbacks[param] = {};
    }

} // namespace Gammou