

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


long getEndNumber(long maxValue) {
	return (long)sqrt((double)maxValue);
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
		return 0;
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


__kernel void findPolindrome(__global Result* result, __constant long *maxValue) {

	int endNumber = get_global_id(0);
	if(endNumber > maxValue)

	for (int startNumber = endNumber; startNumber > 0; startNumber--)
	{
		long number = getSumOfSqrs(startNumber, endNumber, maxValue);

		if (isPolindrome(number) && number > result->polindrome) {
			result->polindrome = number;
			result->minValue = startNumber;
			result->maxValue = endNumber;
		}
	}
};