#ifndef SHAREDFUNCS_H
#define SHAREDFUNCS_H

#include <array>
#include <cinttypes>
#include <math.h>

namespace SyncBlink
{
    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    map(T x, float inMin, float inMax, float outMin, float outMax)
    {
        return (T) (x - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
    }

    template<typename T, int TSIZE, typename TOUT>
    std::array<TOUT, TSIZE> mapa(const std::array<T, TSIZE>& values, float inMin, float inMax, float outMin, float outMax)
    {
        std::array<TOUT, TSIZE> mappedOut = {0};
        for(size_t i = 0; i < values.size(); i++)
        {
            mappedOut[i] = (TOUT) map(values[i], inMin, inMax, outMin, outMax);
        }
        return mappedOut;
    }

    // This function returns the moving average remapped to the new min and max values
    template <typename T, typename TOUT, int TSIZE, int TWINDOW> std::array<TOUT, (TSIZE - TWINDOW)> getScaledMovingAverage(const std::array<T, TSIZE>& values, TOUT outMin, TOUT outMax)
    {
        std::array<float, (TSIZE - TWINDOW)> averages = {0};
        std::array<TOUT, (TSIZE - TWINDOW)> averagesOut = {0};

        float minValue = 0;
        float maxValue = 0;
        for (size_t i = TWINDOW; i < values.size(); i++) {
            float sum = 0.0f;

            for (size_t n = 0; n < TWINDOW; n++) {
                sum += values[i - n];
            }

            const float average = sum / TWINDOW;
            averages[i-TWINDOW] = average;

            if(average > maxValue) maxValue = average;
            if(average < minValue || i == TWINDOW) minValue = average;
        }

        for (size_t i = 0; i < averages.size(); i++) 
        {
            averagesOut[i] = (TOUT)SyncBlink::map(averages[i], minValue, maxValue, (float)outMin, (float)outMax);
        }

        return averagesOut;
    }
}

#endif // SHAREDFUNCS_H