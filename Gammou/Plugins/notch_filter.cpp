

#include "plugin_helper.h"

using namespace Gammou::Sound;

class notch_filter : public sound_component
{

  public:
    notch_filter(const unsigned int channel_count);
    ~notch_filter() {}


    void process(const double input[]) override;
    void initialize_process() override;
    //void on_sample_rate_change(const double new_sample_rate) override;
  private:
    multi_channel_array<double> m_x;
    multi_channel_array<double> m_y;
};

notch_filter::notch_filter(const unsigned int channel_count)
    : sound_component("Notch", 3, 1, channel_count), 
      m_x(this, 5),
      m_y(this, 4)
{
    //	Par defaut, In-0, In1 ,...., Out-0, Out-1, ...
    set_input_name("Input", 0);
    set_input_name("Freq", 1);
    set_input_name("Damping", 2);

    set_output_name("Output", 0);
}

void notch_filter::initialize_process()
{
    for (unsigned int i = 0; i < 5; i++)
        m_x[i] = 0.0;
    for (unsigned int i = 0; i < 4; i++)
        m_y[i] = 0.0;
    
}

void notch_filter::process(const double input[])
{   
    const double x = input[0];
    const double T = get_sample_duration();
    const double w = atan(6.28318530718 * input[1] * T * 0.5) * 2.0 / T;
    const double damping = input[2];

    const double w2 = w*w;

    const double T3 = T*T*T;
    const double T4 = T*T3;
    const double T5 = T*T4;

    const double a1 = w2*T5 + 4.0*T3;
    const double a2 = 5.0*w2*T5 + 4.0*T3;
    const double a3 = 10.0*w2*T5 - 8.0*T3;
    const double a4 = a3;
    const double a5 = 5.0*w2*T5 + 4.0*T3;
    const double a6 = 4.0*T3 + T5*w2;
    const double a7 = 4.0*w*T4*damping + 4.0*T3 + T5*w2;
    const double a8 = 5.0*T5*w2 + 4.0*T3 + 12.0*damping*T4*w;
    const double a9 = 8.0*damping*T4*w - 8.0*T3 + 10.0*T5*w2;
    const double a10 = -8.0*damping*T4*w -8.0*T3 + 10.0*T5*w2;
    const double a11 = -12.0*damping*T4*w + 4.0*T3 + 5.0*T5*w2;
    const double a12 = -4.0*w*T4*damping + 4.0*T3 + T5*w2;


    const double out =
       (1.0 / a7) * (
            (a6 * m_x[4] + a5 * m_x[3] + a4 * m_x[2] + a3 * m_x[1] + a2 * m_x[0] + a1 * x) - 
            (a12 * m_y[3] + a11 * m_y[2] + a10 * m_y[1] + a9 * m_y[0] + a8 * m_output[0]));

    for (unsigned int i = 4; i > 0; --i)
        m_x[i] = m_x[i-1];
    m_x[0] = x;
    for (unsigned int i = 3; i > 0; --i)
        m_y[i] = m_y[i-1]; 
    m_y[0] = m_output[0];

    m_output[0] = out;    
}

EXPORT_DEFAULT_FACTORY(
    notch_filter,      //		Classe
    "notch",           //		Nom
    ComponentCategory::Filter, //		Categorie
    63410              //		Factory Id
)
