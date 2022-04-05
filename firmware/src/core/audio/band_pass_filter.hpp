#ifndef BANDPASSFILTER_H
#define BANDPASSFILTER_H

#include <array>

namespace SyncBlink
{
    /**
     * @brief Bandpass filter for frequencies <= 100 and >= 41000
     *
     */
    class BandPassFilter
    {
    public:
        // http://www.schwietering.com/jayduino/filtuino/index.php?
        // characteristic=bu&passmode=bp&order=2&usesr=usesr&sr=9000&
        // frequencyLow=100&noteLow=&frequencyHigh=4100&noteHigh=&pw=pw&calctype=float&run=Send
        float filter(const float sample)
        {
            _filterValues[0] = _filterValues[1];
            _filterValues[1] = _filterValues[2];
            _filterValues[2] = _filterValues[3];
            _filterValues[3] = _filterValues[4];
            _filterValues[4] = (7.809958825730578535e-1f * sample) + (-0.61050725513825743196f * _filterValues[0]) +
                               (-0.17599206727882477086f * _filterValues[1]) + (1.47818833581520703291f * _filterValues[2]) +
                               (0.29292247730559561880f * _filterValues[3]);
            return (_filterValues[0] + _filterValues[4]) - 2 * _filterValues[2];
        }

    private:
        std::array<float, 5> _filterValues = {{0, 0, 0, 0, 0}};
    };
}

#endif // BANDPASSFILTER_H