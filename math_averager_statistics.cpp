/*
https://jonisalonen.com/2014/efficient-and-accurate-rolling-standard-deviation/

Di= Yi - Ui-1
Ui=Ui-1 + 1/i*Di
Si=Si-1 + ((i-1)/i)*Di^2

    // [ avg' * (n-1) + x ] / n
*/
#include <numeric>
#include <cmath>
#include <limits>
#include <functional>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <type_traits>

template<typename T = float>
struct math
{
    static inline T square(T a) { return a * a; }
};
using mathf = math<float>;

template<typename T, size_t COUNT> 
std::string print( const T (&values)[COUNT] )
{
    std::ostringstream os;
    for(const T& value : values)
    {
        os << value << ", ";
    }

    return os.str();
}

template<typename T, size_t NSAMPLES = 10>
struct Averager
{
    using float_t = float;

    T values[NSAMPLES];
    size_t numValues = 0;
    uint8_t nextEntry = 0;

    float_t stdev = 0.f;
    float_t avg = 0.f;
    float_t variance = 0.f;

    void PrintStats()
    {
        float_t min, max, avg, stdev2;
        GetMinMaxAvg(min, avg, max, stdev2);
        std::cout
            << " #Values: " << numValues
            << " avg: " << avg
            << " min: " << min
            << " max: " << max
            << " s^2: " << variance
            << " s: " << stdev
            << std::endl;
    }
    
    Averager() : values() {}
    ~Averager()
    {
        float_t avg, min, max;
        float_t stdev;
        GetMinMaxAvg(min, avg, max, stdev);
        std::cout
            << "Avg: " << avg
            << " min: " << min
            << " max: " << max
            << " StDev: " << stdev
            << std::endl;
    }
    void PushValue(T value)
    {
        const T oldValue = values[nextEntry];
        size_t localNumValues = numValues + 1;
        
        const float_t oldAvg = avg;
        avg += (value - oldAvg) / localNumValues;
        variance += mathf::square(value - avg) / std::max<int>(1,localNumValues);
        stdev = std::sqrt(variance);

        const float_t distance = std::abs(value - avg);
        //std::cout << " dst: " << distance << std::endl;
        if (numValues < 2 || mathf::square(distance) < 1.5f * variance)
        {
            values[nextEntry] = value;
            numValues = std::min( numValues + 1, NSAMPLES );
            nextEntry = ++nextEntry % NSAMPLES;
        } else {
            std::cout << "Discard(dist:"<< distance << "):" << value << std::endl;
        }
    }
    template<size_t LOCAL_SAMPLES = NSAMPLES>
    void getRange(size_t& beginIndex, size_t& count) const
    {
        count = std::min(numValues, LOCAL_SAMPLES);    
        beginIndex = nextEntry >= count 
            ? nextEntry - count
            : numValues - (count - nextEntry);
    }
    inline T getValue(size_t index) const
    {
            return values[index % numValues];
    }
    template<size_t LOCAL_SAMPLES = NSAMPLES>
    float_t GetAverage()
    {
        size_t i0, count;
        getRange(i0, count);

        float_t accum = float_t(0);
        for ( size_t i = i0; i < i0 + count; ++i )
        {
            accum += getValue(i);
        }
        return accum / count;
    }
    template<size_t LOCAL_SAMPLES = NSAMPLES>
    void GetStDev(float_t& stdev)
    {
        size_t i0, count;
        getRange(i0, count);

        const float_t avg = GetAverage<LOCAL_SAMPLES>();
        float_t stdevAccum = float_t(0);
        for ( size_t i = i0; i < i0 + count; ++i )
        {
            stdevAccum += mathf::square(getValue(i) - avg);
        }
        stdev = stdevAccum / (count - 1);
    }
        template<size_t LOCAL_SAMPLES = NSAMPLES>
    void GetMinMaxAvg(float_t& min, float_t& avg, float_t& max, float_t& stdev)
    {
        size_t i0, count;
        getRange(i0, count);

        float_t accum = float_t(0);
        min = std::numeric_limits<float_t>::max();
        max = -std::numeric_limits<float_t>::max();
        for ( size_t i = i0; i < i0 + count; ++i )
        {
            const T value = getValue(i);
            accum += value;
            min = std::min<float_t>(min, value);
            max = std::max<float_t>(max, value);
        }
        avg = accum / count;
    }


};

int main()
{
    std::vector<int> values{1234,33, 16, 50, 20, 22, 31, 26, 50};
    
    Averager<int> averager;
    size_t COUNT=1000;
    for(int i = 0; i < COUNT; ++i)
    {
        int r0 = rand() % 100;
        int r1 = 0;
        if (r0 > 1)
        {
            r1 = rand() % 100;
        } else {
            r1 = rand() % 5000;
        }
        std::cout << "Value:" << r1 << std::endl;
        averager.PushValue(r1);
        averager.PrintStats();
    }

    for( const auto& value : values )
    {
        averager.PushValue(value);
        averager.PrintStats();
        //std::cout << "StDev: " << averager.GetStandardDeviation() << std::endl;
    }

}
