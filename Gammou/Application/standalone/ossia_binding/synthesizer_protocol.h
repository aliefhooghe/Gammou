#ifndef SYNTHESIZER_PROTOCOL_H_
#define SYNTHESIZER_PROTOCOL_H_


#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>

#include <synthesizer.h>

namespace Gammou {

    class synthesizer_ossia_protocol : 
        public ossia::net::protocol_base 
    {
        public:
            synthesizer_ossia_protocol(Sound::synthesizer *synthesizer);
            ~synthesizer_ossia_protocol();

            void set_device(ossia::net::device_base& dev) override;

            bool pull(ossia::net::parameter_base& param) override;
            bool push(const ossia::net::parameter_base& param) override;
            bool push_raw(const ossia::net::full_parameter_data&) override;
            bool observe(ossia::net::parameter_base& param, bool enable) override;
    
            bool update(ossia::net::node_base&) override;

        private:
            Sound::synthesizer *m_synthesizer;
    };

}   /* namespace Gammou */


#endif