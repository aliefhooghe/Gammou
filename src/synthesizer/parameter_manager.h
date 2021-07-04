#ifndef GAMMOU_PARAMETER_MANAGER_H_
#define GAMMOU_PARAMETER_MANAGER_H_

#include <deque>
#include <algorithm>
#include <stack>
#include <limits>
#include <functional>

namespace Gammou {

    class parameter_manager {

        static constexpr auto default_shape_scale = 1.0f;
        static constexpr auto default_shape_base = 3.0f;

    public:
        using param_id = unsigned int;
        static constexpr param_id INVALID_PARAM = std::numeric_limits<param_id>::max();
        using control_changed_callback = std::function<void()>;

        /**
         * \class parameter
         * \brief Describe a parameter handle, that manage its lifetime
         */
        class parameter {
            friend class parameter_manager;
        public:
            parameter(parameter&& other) noexcept
            :   _mgr{other._mgr}
            {
                _id = other._id;
                other._id = INVALID_PARAM;
            }

            ~parameter() noexcept
            {
                if (_id != INVALID_PARAM)
                    _mgr._free_parameter(_id);
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

            void set_control_changed_callback(control_changed_callback callback) noexcept
            {
                _mgr.set_control_changed_callback(_id, callback);
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

        parameter_manager(float sample_rate, float smooth_characteristic_time = 0.05f) noexcept;

        void set_sample_rate(float sample_rate) noexcept;
        void set_smooth_characteristic_time(float tau) noexcept;
        void process_one_sample() noexcept;

        parameter allocate_parameter(float initial_normalized_value = 0.f);

        void set_parameter_nomalized(param_id param, float value) noexcept;
        void set_parameter_shape_scale(param_id param, float scale) noexcept;
        void set_parameter_shape_base(param_id param, float base) noexcept;

        float get_parameter_nomalized(param_id param) const noexcept;
        float get_parameter_shape_scale(param_id param) const noexcept;
        float get_parameter_shape_base(param_id param) const noexcept;
        const float *get_parameter_value_ptr(param_id param) const noexcept;

        void set_control_changed_callback(param_id param, control_changed_callback callback) noexcept;

    private:
        void _free_parameter(param_id param) noexcept;

        std::stack<unsigned int> _free_params;
        std::deque<float> _parameter_normalized_settings{};
        std::deque<float> _parameter_settings{};
        std::deque<float> _parameter_values{};
        std::deque<float> _shape_scales{};
        std::deque<float> _shape_bases{};
        std::deque< control_changed_callback> _callbacks{};
        float _dt;
        float _smooth_characteristic_time;
    };


}

#endif