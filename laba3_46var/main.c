#define _SVID_SOURCE

#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "math.h"

#include "CL/cl.h";



#pragma warning(disable : 4996)

#define BUFFER_SIZE 255
#define MAX_SOURCE_SIZE 9999999999

typedef struct Result Result;
typedef struct Node Node;

struct Result {
	long polindrome;
	int minValue;
	int maxValue;
};

struct Node
{
	long value;
	Node* back;
	Node* next;
};

char* errCodeToString(int err) {
	switch (err) {
	case CL_SUCCESS:                            return "Success!";
	case CL_DEVICE_NOT_FOUND:               return "Device not found.";
	case CL_DEVICE_NOT_AVAILABLE:        return "Device not available";
	case CL_COMPILER_NOT_AVAILABLE:  return "Compiler not available";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "Memory object allocation failure";
	case CL_OUT_OF_RESOURCES:               return "Out of resources";
	case CL_OUT_OF_HOST_MEMORY:         return "Out of host memory";
	case CL_PROFILING_INFO_NOT_AVAILABLE: return "Profiling information not available";
	case CL_MEM_COPY_OVERLAP:              return "Memory copy overlap";
	case CL_IMAGE_FORMAT_MISMATCH:  return "Image format mismatch";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "Image format not supported";
	case CL_BUILD_PROGRAM_FAILURE:    return "Program build failure";
	case CL_MAP_FAILURE:                        return "Map failure";
	case CL_INVALID_VALUE:                      return "Invalid value";
	case CL_INVALID_DEVICE_TYPE:                return "Invalid device type";
	case CL_INVALID_PLATFORM:                   return "Invalid platform";
	case CL_INVALID_DEVICE:                     return "Invalid device";
	case CL_INVALID_CONTEXT:                    return "Invalid context";
	case CL_INVALID_QUEUE_PROPERTIES:   return "Invalid queue properties";
	case CL_INVALID_COMMAND_QUEUE:      return "Invalid command queue";
	case CL_INVALID_HOST_PTR:                   return "Invalid host pointer";
	case CL_INVALID_MEM_OBJECT:                 return "Invalid memory object";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "Invalid image format descriptor";
	case CL_INVALID_IMAGE_SIZE:                 return "Invalid image size";
	case CL_INVALID_SAMPLER:                    return "Invalid sampler";
	case CL_INVALID_BINARY:                     return "Invalid binary";
	case CL_INVALID_BUILD_OPTIONS:              return "Invalid build options";
	case CL_INVALID_PROGRAM:                    return "Invalid program";
	case CL_INVALID_PROGRAM_EXECUTABLE: return "Invalid program executable";
	case CL_INVALID_KERNEL_NAME:                return "Invalid kernel name";
	case CL_INVALID_KERNEL_DEFINITION:    return "Invalid kernel definition";
	case CL_INVALID_KERNEL:                     return "Invalid kernel";
	case CL_INVALID_ARG_INDEX:                  return "Invalid argument index";
	case CL_INVALID_ARG_VALUE:                  return "Invalid argument value";
	case CL_INVALID_ARG_SIZE:                   return "Invalid argument size";
	case CL_INVALID_KERNEL_ARGS:          return "Invalid kernel arguments";
	case CL_INVALID_WORK_DIMENSION:   return "Invalid work dimension";
	case CL_INVALID_WORK_GROUP_SIZE:  return "Invalid work group size";
	case CL_INVALID_WORK_ITEM_SIZE:     return "Invalid work item size";
	case CL_INVALID_GLOBAL_OFFSET:              return "Invalid global offset";
	case CL_INVALID_EVENT_WAIT_LIST:            return "Invalid event wait list";
	case CL_INVALID_EVENT:                      return "Invalid event";
	case CL_INVALID_OPERATION:                  return "Invalid operation";
	case CL_INVALID_GL_OBJECT:                  return "Invalid OpenGL object";
	case CL_INVALID_BUFFER_SIZE:                return "Invalid buffer size";
	case CL_INVALID_MIP_LEVEL:                  return "Invalid mip-map level";
	default: return "Unknown";
	}
}

long ipow(long base, long exp)
{
	long result = 1;
	for (;;)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		if (!exp)
			break;
		base *= base;
	}

	return result;
}

size_t readSource(char** source_str, const char* fileName) {
	FILE* fp;
	size_t source_size = 0;
	int i;

	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}

	fseek(fp, 0, SEEK_END); // seek to end of file
	int size = ftell(fp); // get current file pointer
	fseek(fp, 0, SEEK_SET);
	*source_str = (char*)malloc(size);
	source_size = fread(*source_str, 1, size, fp);
	fclose(fp);
	return source_size;
}

cl_program build(cl_context context, cl_device_id* devices, size_t numDevices, char* source_str, size_t source_size) {
	cl_int cl_RetCode;
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &cl_RetCode);
	/* скомпилировать программу */
	cl_RetCode = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
	if (cl_RetCode != 0)
	{
		printf("Error build: %s \n", errCodeToString(cl_RetCode));
		size_t len;
		char* logBuffer;
		cl_RetCode = clGetProgramBuildInfo(program, *devices, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
		logBuffer = calloc(len, sizeof(char));
		clGetProgramBuildInfo(program, *devices, CL_PROGRAM_BUILD_LOG, len, logBuffer, NULL);
		printf("Log: %s", logBuffer);
		exit(1);
	}

	return program;
}

void stopwatchWork(void (*onWork)(struct System** systemRef), struct System** param) {

	double  startTime = 0;
	(*onWork)(&(*param));
	double endTime = 0;
	double timeWork = (endTime - startTime);
	printf("Time: %f \n\n", timeWork);
}

long  inputNumber() {
	long input;
	printf("Input number: ");
	scanf("%ld", &input);
	return input;
}

long getEndNumber(long maxValue) {
	return (long)sqrt(maxValue);
}

long getSumOfSqrsNode(long endValue, long maxValue) {

	Node* endNode = malloc(sizeof(Node));
	endNode->value = ipow(endValue, 2);
	endNode->next = NULL;
	long sum = endNode->value;
	Node* lastNode = endNode;
	for (int i = endNode - 1; i > 0; i)
	{
		Node* node = malloc(sizeof(Node));
		node->value = ipow(endValue - i, 2);
		lastNode->back = node;
		node->next = lastNode;
		lastNode = node;
		long tmpSum = sum + lastNode->value;

		if (tmpSum > maxValue)
		{
			return sum;
		}
		else
		{
			sum = tmpSum;
		}
	}


	return sum;
}

long getSumOfSqrs(long startValue, long endValue, long maxValue) {

	long sum = ipow(endValue, 2);
	for (int i = endValue - 1; i >= startValue; i--)
	{
		long tmpSum = sum + ipow(i, 2);

		if (tmpSum > maxValue)
		{
			return sum;
		}
		sum = tmpSum;
	}

	return sum;
}


void splitNumber(Node** first, Node** last, long number) {
	long tmpNumber = number;

	Node* firstNode = malloc(sizeof(Node));
	Node* lastNode = firstNode;

	firstNode->value = tmpNumber % 10;
	tmpNumber = tmpNumber / 10;

	do {
		int digit = tmpNumber % 10;
		Node* node = malloc(sizeof(Node));
		node->value = digit;
		node->back = lastNode;
		lastNode->next = node;
		lastNode = node;

		tmpNumber = tmpNumber / 10;
	} while (tmpNumber != 0);

	*first = firstNode;
	*last = lastNode;
}

_Bool checkPolindrome(Node* frontPointer, Node* tailPointer) {
	if (frontPointer == NULL || tailPointer == NULL)
	{
		printf("Incorrcet args of fun checkPolindrome");
		exit(0);
	}
	do
	{
		if (frontPointer->value != tailPointer->value)
		{
			return 0;
		}

		Node* tmpFrontPointer = frontPointer;
		Node* tmpTailPointer = tailPointer;
		frontPointer = frontPointer->next;
		if (frontPointer == tailPointer)
		{
			free(tmpFrontPointer);
			free(frontPointer);
			break;
		}
		tailPointer = tailPointer->back;
		free(tmpFrontPointer);
		free(tmpTailPointer);
	} while (frontPointer != tailPointer);

	return 1;
}

_Bool isPolindrome(long number) {

	if (abs(number) < 10)
	{
		return 1;
	}

	Node* frontPointer = NULL;
	Node* tailPointer = NULL;

	splitNumber(&frontPointer, &tailPointer, number);

	return checkPolindrome(frontPointer, tailPointer);
}

Result* findLargestPolindrome(long maxValue) {


	Result* result = malloc(sizeof(Result));
	result->polindrome = 0;

	for (int endNumber = getEndNumber(maxValue); endNumber > 0; endNumber--)
	{
		for (int startNumber = endNumber; startNumber > 0; startNumber--)
		{
			long number = getSumOfSqrs(startNumber, endNumber, maxValue);

			if (isPolindrome(number) && number > result->polindrome) {
				result->polindrome = number;
				result->minValue = startNumber;
				result->maxValue = endNumber;
			}
		}
	}

	return result;
}

int* createArrayNumbers(long endNumber) {
	int* numbers = malloc(endNumber * sizeof(int));
	for (int i = 0; i < endNumber; i++)
	{
		numbers[i] = i + 1;
	}
	return numbers;
}

cl_context createContext(const cl_device_id* devices, cl_uint numDevices) {
	cl_int cl_RetCode;
	cl_context context = clCreateContext(NULL, numDevices, &devices, NULL, NULL, &cl_RetCode);
	if (cl_RetCode != 0)
	{
		printf("Error of createContext : %s", errCodeToString(cl_RetCode));
		exit(1);
	}
	return context;
}

cl_kernel createKernel(cl_program program, const char* name) {
	cl_int cl_RetCode;
	cl_kernel kernel = clCreateKernel(program, name, &cl_RetCode);
	if (cl_RetCode != 0)
	{
		printf("Error of createKernel : %s", errCodeToString(cl_RetCode));
		exit(1);
	}
	return kernel;
}

int* getSizeNumbers(cl_context context, const cl_device_id* devices, int numDevices, long* numbers, long numNumbers) {
	
	cl_int cl_RetCode;

	char* source;
	size_t sizeSource = readSource(&source, "getSizeNumbers.cl");
	cl_program program = build(context, devices, numDevices, source, sizeSource);
	cl_kernel kernel = createKernel(program, "getSizeNumbers");
	cl_command_queue command_queue = clCreateCommandQueue(context, *devices, 0, &cl_RetCode);

	long sizeOfNumbers = numNumbers * sizeof(long);
	long sizeOfLengthNumbers = numNumbers * sizeof(long);

	cl_mem lengthNumbersObj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeOfLengthNumbers, NULL, &cl_RetCode);
	cl_mem numbersObj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeOfNumbers, NULL, &cl_RetCode);
	cl_mem numNumbersObj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(long), NULL, &cl_RetCode);

	/* записать данные в буфер */
	cl_RetCode = clEnqueueWriteBuffer(command_queue, numbersObj, CL_TRUE, 0, sizeOfNumbers, numbers, 0, NULL, NULL);
	cl_RetCode = clEnqueueWriteBuffer(command_queue, numNumbersObj, CL_TRUE, 0, sizeOfNumbers, &numNumbers, 0, NULL, NULL);

	/* устанавливаем параметр */
	cl_RetCode = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&lengthNumbersObj);
	cl_RetCode = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&numbersObj);
	cl_RetCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&numNumbersObj);



	size_t sumWorkItems = 0;
	size_t clWorkItemSizes[1] = { 0 };
	for (int i = 0; i < numDevices; i++)
	{
		cl_RetCode = clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, 3 * sizeof(size_t), &clWorkItemSizes, NULL);
		sumWorkItems += clWorkItemSizes[0];
	}

	size_t global_work_size[1] = { 0 };
	if (sumWorkItems < numNumbers)
	{
		global_work_size[0] = sumWorkItems;
	}
	else
	{
		global_work_size[0] = numNumbers;
	}

	/* выполнить кернел */
	cl_RetCode = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	if (cl_RetCode != 0)
	{
		printf("Error of getSizeNumbers (clEnqueueNDRangeKernel) :%s", errCodeToString(cl_RetCode));
		exit(1);
	}
	/* считать данные из буфера */
	int* lengthNumbers = malloc(numNumbers * sizeof(int));
	for (size_t i = 0; i < numNumbers; i++)
	{
		lengthNumbers[i] = 0;
	}
	cl_RetCode = clEnqueueReadBuffer(command_queue, lengthNumbersObj, CL_TRUE, 0, sizeOfNumbers, lengthNumbers, 0, NULL, NULL);
	if (cl_RetCode != 0)
	{
		printf("Error of getSizeNumbers (clEnqueueReadBuffer) :%s", errCodeToString(cl_RetCode));
		exit(1);
	}
	return lengthNumbers;
}

void getPlatformsIds(cl_platform_id** cl_PlatformIds, cl_uint* cl_NumPlatforms) {
	cl_int cl_RetCode;

	cl_RetCode = clGetPlatformIDs(0, NULL, cl_NumPlatforms);
	if (cl_NumPlatforms == 0)
	{
		printf("Platforms not founded");
		return;
	}
	*cl_PlatformIds = malloc(sizeof(cl_platform_id));
	cl_RetCode = clGetPlatformIDs(*cl_NumPlatforms, *cl_PlatformIds, NULL);
}

void printPlatfomsNames(cl_platform_id* cl_PlatformIds, cl_uint cl_NumPlatforms) {
	const int bufferSize = 255;
	char* name = malloc(bufferSize * sizeof(char));
	for (int i = 0; i < cl_NumPlatforms; i++)
	{
		clGetPlatformInfo(cl_PlatformIds[i], CL_PLATFORM_NAME, bufferSize, name, NULL);
		printf("Name platform: %s \n", name);
	}
	free(name);
}


Result* clFindLargetPolindrome(long maxValue) {
	cl_platform_id* cl_PlatformIds = NULL;
	cl_uint cl_NumPlatforms;
	getPlatformsIds(&cl_PlatformIds, &cl_NumPlatforms);
	printPlatfomsNames(cl_PlatformIds, cl_NumPlatforms);

	cl_device_id** cl_DevIds = (cl_device_id**)malloc(cl_NumPlatforms * sizeof(cl_device_id*));
	cl_uint* cl_QtyDevices = (cl_uint*)malloc(cl_NumPlatforms * sizeof(cl_uint));

	cl_uint sumOfDevices = 0;
	cl_int cl_RetCode;
	for (int ui = 0; ui < cl_NumPlatforms; ui++) {
		cl_RetCode = clGetDeviceIDs(cl_PlatformIds[ui], CL_DEVICE_TYPE_ALL, 0, NULL, &cl_QtyDevices[ui]);
		if (cl_QtyDevices[ui] > 0) {
			sumOfDevices += cl_QtyDevices[ui];
			cl_DevIds[ui] = (cl_device_id*)malloc(cl_QtyDevices[ui] * sizeof(cl_device_id));
			cl_RetCode = clGetDeviceIDs(cl_PlatformIds[ui], CL_DEVICE_TYPE_ALL, cl_QtyDevices[ui], cl_DevIds[ui], NULL);
		}
	}
	cl_device_id* devices = malloc(sumOfDevices * sizeof(cl_device_id));
	int devicePointer = 0;
	for (int platformIndex = 0; platformIndex < cl_NumPlatforms; platformIndex++)
	{
		for (int deviceIdx = 0; deviceIdx < cl_QtyDevices[platformIndex]; deviceIdx++)
		{
			devices[devicePointer] = cl_DevIds[platformIndex][deviceIdx];
			devicePointer++;
		}
	}



	Result* result = malloc(sizeof(Result));
	result->polindrome = 0;
	int endNumber = getEndNumber(maxValue);
	int* numbers = createArrayNumbers(endNumber);
	sumOfDevices = 1;
	cl_context context = clCreateContext(NULL, sumOfDevices, devices, NULL, NULL, &cl_RetCode);
	int* sizesNumbers = getSizeNumbers(context, devices, sumOfDevices, numbers, endNumber);


	return result;
}

void printResult(Result result) {
	printf("Polindrome founded: %ld \n", result.polindrome);
	printf("Sequence: \n");
	printf("\t minValue: %ld\n", result.minValue);
	printf("\t maxValue: %ld\n", result.maxValue);
}




	


int main() {

	long N = inputNumber();

	Result* result = findLargestPolindrome(N);
	printResult(*result);

	free(result);

	
	clFindLargetPolindrome(N);

	//size_t cl_WorkGroupSize;
	//cl_uint cl_ComputeUnits;
	//size_t cl_WorkItemSizes[3] = { 0, 0, 0 };
	//cl_uint cl_MemAvailable;


	//cl_RetCode = clGetDeviceInfo(cl_DevIds[0][0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &cl_WorkGroupSize, NULL);
	//cl_RetCode = clGetDeviceInfo(cl_DevIds[0][0], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &cl_ComputeUnits, NULL);
	//cl_RetCode = clGetDeviceInfo(cl_DevIds[0][0], CL_DEVICE_MAX_WORK_ITEM_SIZES, 3 * sizeof(size_t), &cl_WorkItemSizes, NULL);
	//cl_RetCode = clGetDeviceInfo(cl_DevIds[0][0], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &cl_MemAvailable, NULL);

	//cl_program program = NULL;
	//cl_kernel kernel = NULL;

	//FILE* fp;
	//const char fileName[] = "findPolindrome.cl";
	//size_t source_size;
	//char* source_str;
	//int i;

	//fp = fopen(fileName, "r");
	//if (!fp) {
	//	fprintf(stderr, "Failed to load kernel.\n");
	//	exit(1);
	//}

	//fseek(fp, 0, SEEK_END); // seek to end of file
	//int size = ftell(fp); // get current file pointer
	//fseek(fp, 0, SEEK_SET);
	//source_str = (char*)malloc(size);
	//source_size = fread(source_str, 1, size, fp);
	//fclose(fp);

	//cl_device_id* device = cl_DevIds[0][0];
	///* создать контекст */
	//cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &cl_RetCode);

	///* создаем команду */
	//cl_command_queue command_queue = clCreateCommandQueue(context, device, 0, &cl_RetCode);
	///* создать бинарник из кода программы */
	//program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &cl_RetCode);
	//free(source_str);
	///* скомпилировать программу */
	//cl_RetCode = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	//if (cl_RetCode != 0 )
	//{
	//	printf("Error compilation: %d", cl_RetCode);
	//	size_t len;
	//	char* logBuffer;
	//	cl_RetCode = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
	//	logBuffer = calloc(len, sizeof(char));
	//	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, len, logBuffer, NULL);
	//	printf("Log: %s", logBuffer);
	//}
	//
	///* создать кернел */
	//kernel = clCreateKernel(program, "findPolindrome", &cl_RetCode);

	//cl_mem resultObj = NULL;
	//Result* clResult = (Result*)malloc(sizeof(Result));
	//cl_mem maxValueObj = NULL;
	//long *maxValue = &N;

	///* создать буфер */
	//resultObj = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(Result), NULL, &cl_RetCode);
	//resultObj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Result), NULL, &cl_RetCode);

	///* записать данные в буфер */
	//cl_RetCode = clEnqueueWriteBuffer(command_queue, resultObj, CL_TRUE, 0, sizeof(Result), clResult, 0, NULL, NULL);
	//cl_RetCode = clEnqueueWriteBuffer(command_queue, maxValueObj, CL_TRUE, 0, sizeof(int), maxValue, 0, NULL, NULL);

	///* устанавливаем параметр */
	//cl_RetCode = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&resultObj);
	//cl_RetCode = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&maxValueObj);


	//size_t global_work_size[1] = { N };

	///* выполнить кернел */
	//cl_RetCode = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

	///* считать данные из буфера */
	//cl_RetCode = clEnqueueReadBuffer(command_queue, resultObj, CL_TRUE, 0, sizeof(Result), clResult, 0, NULL, NULL);

	return(0);
}