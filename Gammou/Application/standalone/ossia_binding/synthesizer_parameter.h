#ifndef SYNTHESIZER_PARAMETER_H_
#define SYNTHESIZER_PARAMETER_H_

#include <ossia/network/value/value.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>

#include <synthesizer.h>

namespace Gammou {

    class synthesizer_ossia_parameter : public ossia::net::parameter_base {

        public:
            synthesizer_ossia_parameter(
                ossia::net::node_base& node,
                Sound::synthesizer *synthesizer,
                const unsigned int parameter_id);

            void pull_value() override;
            ossia::value value() const override;
            
            ossia::net::parameter_base& push_value(const ossia::value& val) override;
            ossia::net::parameter_base& push_value(ossia::value&& val) override;
            ossia::net::parameter_base& push_value() override;

            ossia::net::parameter_base& set_value(const ossia::value& val) override;
            ossia::net::parameter_base& set_value(ossia::value&& v) override;

            ossia::val_type get_value_type() const override;
            ossia::net::parameter_base& set_value_type(ossia::val_type) override {return *this; }

            ossia::access_mode get_access() const override;
            ossia::net::parameter_base& set_access(ossia::access_mode) override { return *this; }

            const ossia::domain& get_domain() const override;
            ossia::net::parameter_base& set_domain(const ossia::domain&) override { return *this; }

            ossia::bounding_mode get_bounding() const override;
            ossia::net::parameter_base& set_bounding(ossia::bounding_mode) override { return *this; }
    
        private:
            auto& get_protocol() { return get_node().get_device().get_protocol(); }

            Sound::synthesizer *m_synthesizer;
            const unsigned int m_parameter_id;
            ossia::domain m_domain;
    };


} /* Gammou */

#endif