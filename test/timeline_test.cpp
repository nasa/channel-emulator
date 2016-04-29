#include <cmath>
#include <iostream>
#include <iomanip>
#include <TimeLine.hpp>
#include "nd_error.hpp"

using namespace std;

int main() {
	nasaCE::TimeLine<int> rates(100);

	ACE_Time_Value time1(10), time2(20), time3(30);

	rates.append(time1, 120);
	rates.append(time2, 150);
	rates.append(time3, 180);

	ACE_Time_Value ttime1(10),ttime2(11),ttime3(20),ttime4(25),ttime5(30),ttime6(100);

	cout << rates[ttime1] << ", " << rates[ttime2] << ", "
		<< rates[ttime3] << ", " << rates[ttime4] << ", "
		<< rates[ttime5] << ", " << rates[ttime6] << endl;

	return 0;

}
