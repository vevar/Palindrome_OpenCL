

__kernel void getSizeNumbers(__global int* lengthNumbers, __global int* numbers, __global int* numNumbers) {
	int index = get_global_id(0);

	__private int number = numbers[index];
	__private int numDigits = 0;

	do {
		numDigits++;
		number = number / 10;
	} while (number > 0);

	lengthNumbers[index] = numDigits;
}