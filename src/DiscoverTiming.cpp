// MIT License
//
// Copyright (c) 2021 Daniel Robertson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string>
#include <algorithm>
#include <thread>
#include <iomanip>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <sched.h>
#include "../include/HX711.h"

int main(int argc, char** argv) {

    using namespace std;
    using namespace std::chrono;
    using namespace HX711;

    const char* const err = "Usage: [DATA PIN] [CLOCK PIN] [SAMPLES]";

    if(argc != 4) {
        cout << err << endl;
        return EXIT_FAILURE;
    }

    const int dataPin = stoi(argv[1]);
    const int clockPin = stoi(argv[2]);
    const int samples = stoi(argv[3]);

    struct sched_param schParams = {
        sched_get_priority_max(SCHED_FIFO)
    };

    ::pthread_setschedparam(
        pthread_self(),
        SCHED_FIFO,
        &schParams);

    Discovery dx(dataPin, clockPin, Rate::HZ_80);

    const TimingCollection timings = dx.getTimings(samples);
    const TimingCollection::Stats waitTimes = timings.getWaitTimeStats();
    const TimingCollection::Stats convTimes = timings.getConversionTimeStats();
    const TimingCollection::Stats totalTimes = timings.getTotalTimeStats();

    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);

    cout << "Wait Times" << endl;
    cout << "Min: " << waitTimes.min << " us" << endl;
    cout << "Max: " << waitTimes.max << " us" << endl;
    cout << "Med: " << waitTimes.med << " us" << endl;
    cout << "Std: " << waitTimes.std << " us" << endl;
    cout << endl;

    cout << "Conversion Times" << endl;
    cout << "Min: " << convTimes.min << " us" << endl;
    cout << "Max: " << convTimes.max << " us" << endl;
    cout << "Med: " << convTimes.med << " us" << endl;
    cout << "Std: " << convTimes.std << " us" << endl;
    cout << endl;

    cout << "Total Times" << endl;
    cout << "Min: " << totalTimes.min << " us" << endl;
    cout << "Max: " << totalTimes.max << " us" << endl;
    cout << "Med: " << totalTimes.med << " us" << endl;
    cout << "Std: " << totalTimes.std << " us" << endl;
    cout << endl;

    cout << endl;

    cout << "Total,Wait,Conversion,Value" << endl;

    for(const auto& tr : timings) {

        const double totalTime = duration_cast<microseconds>(tr.getTotalTime()).count();
        const double waitTime = duration_cast<microseconds>(tr.getWaitTime()).count();
        const double convTime = duration_cast<microseconds>(tr.getConversionTime()).count();

        cout    
            << totalTime
            << "," << waitTime
            << "," << convTime
            << "," << tr.v
            << (waitTimes.inRange(waitTime) ? "" : "*")
            << (convTimes.inRange(convTime) ? "" : "#")
            << endl
            ;

    }

    return EXIT_SUCCESS;

}