#include "synthesizer.h"

namespace gammou {

























































const double synthesizer::m_note_frequencies[127] = {
		 8.1757989156,
		 8.661957218,
		 9.1770239974,
		 9.7227182413,
		 10.3008611535,
		 10.9133822323,
		 11.5623257097,
		 12.2498573744,
		 12.9782717994,
		 13.75,
		 14.5676175474,
		 15.4338531643,
		 16.3515978313,
		 17.3239144361,
		 18.3540479948,
		 19.4454364826,
		 20.6017223071,
		 21.8267644646,
		 23.1246514195,
		 24.4997147489,
		 25.9565435987,
		 27.5,
		 29.1352350949,
		 30.8677063285,
		 32.7031956626,
		 34.6478288721,
		 36.7080959897,
		 38.8908729653,
		 41.2034446141,
		 43.6535289291,
		 46.249302839,
		 48.9994294977,
		 51.9130871975,
		 55,
		 58.2704701898,
		 61.735412657,
		 65.4063913251,
		 69.2956577442,
		 73.4161919794,
		 77.7817459305,
		 82.4068892282,
		 87.3070578583,
		 92.4986056779,
		 97.9988589954,
		 103.826174395,
		 110,
		 116.5409403795,
		 123.470825314,
		 130.8127826503,
		 138.5913154884,
		 146.8323839587,
		 155.563491861,
		 164.8137784564,
		 174.6141157165,
		 184.9972113558,
		 195.9977179909,
		 207.65234879,
		 220,
		 233.081880759,
		 246.9416506281,
		 261.6255653006,
		 277.1826309769,
		 293.6647679174,
		 311.1269837221,
		 329.6275569129,
		 349.228231433,
		 369.9944227116,
		 391.9954359818,
		 415.3046975799,
		 440,
		 466.1637615181,
		 493.8833012561,
		 523.2511306012,
		 554.3652619537,
		 587.3295358348,
		 622.2539674442,
		 659.2551138257,
		 698.456462866,
		 739.9888454233,
		 783.9908719635,
		 830.6093951599,
		 880,
		 932.3275230362,
		 987.7666025122,
		 1046.5022612024,
		 1108.7305239075,
		 1174.6590716696,
		 1244.5079348883,
		 1318.5102276515,
		 1396.912925732,
		 1479.9776908465,
		 1567.981743927,
		 1661.2187903198,
		 1760,
		 1864.6550460724,
		 1975.5332050245,
		 2093.0045224048,
		 2217.461047815,
		 2349.3181433393,
		 2489.0158697767,
		 2637.020455303,
		 2793.825851464,
		 2959.9553816931,
		 3135.963487854,
		 3322.4375806396,
		 3520,
		 3729.3100921447,
		 3951.066410049,
		 4186.0090448096,
		 4434.92209563,
		 4698.6362866785,
		 4978.0317395533,
		 5274.0409106059,
		 5587.6517029281,
		 5919.9107633862,
		 6271.926975708,
		 6644.8751612791,
		 7040,
		 7458.6201842894,
		 7902.132820098,
		 8372.0180896192,
		 8869.8441912599,
		 9397.2725733571,
		 9956.0634791066,
		 10548.0818212118,
		 11175.3034058561};


}  // namespace gammou