#include "frequency_map.h"
//Used unorderd map to assign frequency to unique characters....
std::unordered_map<char, int> buildFrequencyMap(const std::string& message)
{
    std::unordered_map<char, int> freqMap;
    for (char c : message)
    {
        freqMap[c]++;
    }
    return freqMap;
}
