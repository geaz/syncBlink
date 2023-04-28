#ifndef SHAREDFUNCS_H
#define SHAREDFUNCS_H

#include <array>

namespace SyncBlink
{
    inline float mapf(float x, float inMin, float inMax, float outMin, float outMax)
    {
        return (x - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
    }

    template <typename T, int U, typename S, int V> std::array<T, U> mapa(std::array<S, V>& values, T outMax)
    {
        std::array<T, U> mappedValues;
        std::array<S, mappedValues.size()> combinedValues;

        S originalMaxValue = 0;
        float splitSize = (float)values.size() / mappedValues.size();
        for (uint32_t i = 0; i < mappedValues.size(); i++)
        {
            combinedValues[i] = 0;
            for (uint32_t j = 0; j < splitSize; j++)
            {
                combinedValues[i] += values[j + (i * splitSize)];
            }
            combinedValues[i] /= splitSize;
            originalMaxValue = combinedValues[i] > originalMaxValue ? combinedValues[i] : originalMaxValue;
        }

        for (uint32_t i = 0; i < mappedValues.size(); i++)
        {
            mappedValues[i] = (T)mapf(combinedValues[i], 0, originalMaxValue, 0, outMax);
        }

        return mappedValues;
    }
}

#endif // SHAREDFUNCS_H