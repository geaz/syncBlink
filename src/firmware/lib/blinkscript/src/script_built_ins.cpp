#include "script_built_ins.hpp"

#include "blink_script.hpp"
#include "program/model/objects/native_function_object.hpp"
#include "program/model/value.hpp"

#include <Arduino.h>
#include <color.hpp>
#include <math.h>

namespace SyncBlink
{
    namespace BuiltIns
    {
        void println(VM& vm)
        {
            vm.addNativeFun("println", std::make_shared<NativeFunObj>(
                                           [](std::vector<Value> args) {
                                               if (args[0].getType() == ValueType::NUMBER)
                                               {
                                                   Serial.print("[SCRIPT] ");
                                                   Serial.println(args[0].number);
                                               }
                                               else if (args[0].getType() == ValueType::OBJECT)
                                               {
                                                   auto strObj = args[0].object;
                                                   if (strObj->getType() == ObjectType::STRING)
                                                   {
                                                       Serial.print("[SCRIPT] ");
                                                       Serial.println(static_cast<StringObj*>(strObj)->getString().c_str());
                                                   }
                                               }
                                               return Value();
                                           },
                                           1));
        }

        void setNumArray(VM& vm)
        {
            vm.addNativeFun("setNumArray", std::make_shared<NativeFunObj>(
                                           [](std::vector<Value> args) {
                                               if(args[0].getType() == ValueType::OBJECT && args[0].object->getType() == ObjectType::ARRAY)
                                               {
                                                    auto arrayObj = args[0].getObject<ArrayObj>();
                                                    arrayObj->getValues().resize((uint16_t) args[1].number);                                                    
                                                    std::fill(arrayObj->getValues().begin(), arrayObj->getValues().end(), Value(args[2].number));
                                               }
                                               return Value();
                                           },
                                           3));
        }

        void setLinearGroups(BlinkScript& blinkScript, VM& vm)
        {
            vm.addNativeFun("setLinearGroupsOf", std::make_shared<NativeFunObj>(
                                                     [&blinkScript](std::vector<Value> args) {
                                                         uint32_t countPerGroup = (uint32_t)args[0].number;
                                                         for (uint32_t i = 0; i < blinkScript.getLed().getLedCount() / countPerGroup; i++)
                                                         {
                                                             std::vector<uint16_t> ledIndices;
                                                             for (uint16_t j = 0; j < countPerGroup; j++)
                                                             {
                                                                 ledIndices.push_back(i + (j * countPerGroup));
                                                             }
                                                             blinkScript.getLed().setGroup(i, ledIndices);
                                                         }
                                                         return Value();
                                                     },
                                                     1));
        }

        void setGroups(BlinkScript& blinkScript, VM& vm)
        {
            vm.addNativeFun("setGroupsOf", std::make_shared<NativeFunObj>(
                                               [&blinkScript](std::vector<Value> args) {
                                                   uint32_t countPerGroup = (uint32_t)args[0].number;
                                                   uint32_t groupCount = blinkScript.getLed().getLedCount() / countPerGroup;
                                                   for (uint32_t i = 0; i < groupCount; i++)
                                                   {
                                                       std::vector<uint16_t> ledIndices;
                                                       for (uint16_t j = 0; j < countPerGroup; j++)
                                                       {
                                                           if (j % 2 == 0) ledIndices.push_back((i + j * groupCount));
                                                           else
                                                               ledIndices.push_back((j + 1) * groupCount - (i + 1));
                                                       }
                                                       blinkScript.getLed().setGroup(i, ledIndices);
                                                   }
                                                   return Value();
                                               },
                                               1));
        }

        void clearGroups(BlinkScript& blinkScript, VM& vm)
        {
            vm.addNativeFun("clearGroups", std::make_shared<NativeFunObj>(
                                               [&blinkScript](std::vector<Value> args) {
                                                   blinkScript.getLed().clearGroups();
                                                   return Value();
                                               },
                                               0));
        }

        void setDelay(BlinkScript& blinkScript, VM& vm)
        {
            vm.addNativeFun("setDelay", std::make_shared<NativeFunObj>(
                                            [&blinkScript](std::vector<Value> args) {
                                                float delay = args[0].number;
                                                blinkScript.setDelay(delay);
                                                return Value();
                                            },
                                            1));
        }

        void getLed(BlinkScript& blinkScript, VM& vm)
        {
            vm.addNativeFun("getLed", std::make_shared<NativeFunObj>(
                                          [&blinkScript](std::vector<Value> args) {
                                              float index = args[0].number;
                                              return Value((float)blinkScript.getLed().getLed(index));
                                          },
                                          1));
        }

        void setLeds(BlinkScript& blinkScript, VM& vm)
        {
            vm.addNativeFun("setLeds",
                            std::make_shared<NativeFunObj>(
                                [&blinkScript](std::vector<Value> args) {
                                    auto arrayVal = args[0];
                                    if (arrayVal.getType() == ValueType::OBJECT && arrayVal.object->getType() == ObjectType::ARRAY)
                                    {
                                        auto arrayObj = static_cast<ArrayObj*>(arrayVal.object);
                                        for (uint32_t i = 0; i < arrayObj->getValues().size(); i++)
                                        {
                                            blinkScript.getLed().setLed(i, Color::fromHex(arrayObj->getValues()[i].number));
                                        }
                                    }
                                    return Value();
                                },
                                1));
        }

        void setAllLeds(BlinkScript& blinkScript, VM& vm)
        {
            vm.addNativeFun("setAllLeds", std::make_shared<NativeFunObj>(
                                              [&blinkScript](std::vector<Value> args) {
                                                  float hexColor = args[0].number;
                                                  blinkScript.getLed().setAllLeds(Color::fromHex(hexColor));
                                                  return Value();
                                              },
                                              1));
        }

        void map(VM& vm)
        {
            vm.addNativeFun("map", std::make_shared<NativeFunObj>(
                                       [](std::vector<Value> args) {
                                           float x = args[0].number;
                                           float inMin = args[1].number;
                                           float inMax = args[2].number;
                                           float outMin = args[3].number;
                                           float outMax = args[4].number;

                                           return Value((x - inMin) / (inMax - inMin) * (outMax - outMin) + outMin);
                                       },
                                       5));
        }

        void xrgb(VM& vm)
        {
            vm.addNativeFun("xrgb", std::make_shared<NativeFunObj>(
                                        [](std::vector<Value> args) {
                                            uint32_t r = args[0].number;
                                            uint32_t g = args[1].number;
                                            uint32_t b = args[2].number;

                                            return Value((float)((r << 16) + (g << 8) + b));
                                        },
                                        3));
        }

        void xhsv(VM& vm)
        {
            vm.addNativeFun("xhsv", std::make_shared<NativeFunObj>(
                                        [](std::vector<Value> args) {
                                            float h = args[0].number;
                                            float s = args[1].number;
                                            float v = args[2].number;

                                            float chroma = v * s;
                                            float hPrime = fmod(h / 60.0, 6);
                                            float x = chroma * (1 - fabs(fmod(hPrime, 2) - 1));
                                            float m = v - chroma;

                                            float r = 0, g = 0, b = 0;
                                            if (0 <= hPrime && hPrime < 1)
                                            {
                                                r = chroma;
                                                g = x;
                                                b = 0;
                                            }
                                            else if (1 <= hPrime && hPrime < 2)
                                            {
                                                r = x;
                                                g = chroma;
                                                b = 0;
                                            }
                                            else if (2 <= hPrime && hPrime < 3)
                                            {
                                                r = 0;
                                                g = chroma;
                                                b = x;
                                            }
                                            else if (3 <= hPrime && hPrime < 4)
                                            {
                                                r = 0;
                                                g = x;
                                                b = chroma;
                                            }
                                            else if (4 <= hPrime && hPrime < 5)
                                            {
                                                r = x;
                                                g = 0;
                                                b = chroma;
                                            }
                                            else if (5 <= hPrime && hPrime < 6)
                                            {
                                                r = chroma;
                                                g = 0;
                                                b = x;
                                            }
                                            else
                                            {
                                                r = 0;
                                                g = 0;
                                                b = 0;
                                            }

                                            r += m;
                                            g += m;
                                            b += m;

                                            uint32_t normR = r * 255;
                                            uint32_t normG = g * 255;
                                            uint32_t normB = b * 255;

                                            return Value((float)((normR << 16) + (normG << 8) + normB));
                                        },
                                        3));
        }
    }
}