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
        // http://www.schwietering.com/jayduino/filtuino/index.php?characteristic=be&passmode=bp&order=2&
        // usesr=usesr&sr=9000&frequencyLow=440&noteLow=&frequencyHigh=4100&noteHigh=&pw=pw&calctype=float&run=Send
        float filter(const float sample)
        {
            _filterValues[0] = _filterValues[1];
			_filterValues[1] = _filterValues[2];
			_filterValues[2] = _filterValues[3];
			_filterValues[3] = _filterValues[4];
			_filterValues[4] = (6.814796055584133594e-1f * sample)
				 + (-0.43969570448650652228f * _filterValues[0])
				 + (0.01964831318625870349f * _filterValues[1])
				 + (1.28577378103135009368f * _filterValues[2])
				 + (-0.03448462098540860943f * _filterValues[3]);
			return (_filterValues[0] + _filterValues[4]) - 2 * _filterValues[2];
        }

    private:
        std::array<float, 5> _filterValues = {{0, 0, 0, 0, 0}};
    };
}

#endif // BANDPASSFILTER_H