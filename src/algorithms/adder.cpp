#include "utility.h"

void Adder(ThreadedBrain &brain)
{
	bool ok;

	int first = 0;
	int second = 0;

	int bits = brain.getInteger("Bits", "Number of bits", 8, &ok);
	if (!ok)
		return;

	const int count = brain.getInteger("Count", "Count of numbers to add.\n 1 or 2", 1, &ok);
	if (!ok)
		return;

	if ((count != 1) && (count != 2))
	{
		brain.log("Invalid count!");
		return;
	}

	if (count == 2)
	{
		first = brain.getInteger("Integer", "Number to add", 0, &ok);
		if (!ok)
			return;
	}

	second = brain.getInteger("Integer", "Additional number", 0, &ok);
	if (!ok)
		return;

	if (count != 2)
	{
		//Read the number at the previous position
		brain.log("Reading the saved number");
		goTo(brain, 0, 1);
		first = readNumber(brain, bits);
		goTo(brain, 0, -1);
		brain.log(QString("Read %1").arg(first));
	}
	else
	{
		brain.log(QString("Writing %1").arg(first));
		goTo(brain, 0, 1);
		writeNumber(brain, first, bits);
		goTo(brain, 0, -1);
	}


	brain.log(QString("Writing %1").arg(second));
	writeNumber(brain, second, bits);
	goTo(brain, 0, -1);

	brain.log(
				QString("Expected result is: %1 + %2 = <b>%3</n>")
				.arg(first)
				.arg(second)
				.arg(first + second));

	//Clear the first carry input
	brain.setDirectionalTile(Qt::white, {0, 3});

	//Adder loop
	//The inputs are read from (0,1), (0,2)
	//The result is written at (0,0)
	//The carry is saved at (1,3)
	brain.log("Calculating");
	for (int i = 0; (i < bits) && brain; ++i)
	{
		//Read the two bits and the carry
		const int a = isSensorDark(brain, {0,2}) ? 1 : 0;
		const int b = isSensorDark(brain, {0,1}) ? 1 : 0;
		const int c = isSensorDark(brain, {0,3}) ? 1 : 0;

		const int r = a + b + c;
		//Result
		brain.setDirectionalTile((r & 1) ? Qt::green : QColor{240,255,240}, {0,0});
		//Carry
		brain.setDirectionalTile((r & 2) ? Qt::blue : QColor{240,240,255}, {1,3});

		brain.move();
	}

	brain.move(-bits);

	brain.log("Reading the result");
	const int result = readNumber(brain, bits);
	goTo(brain, 0, -1);

	brain.log(QString("Calculated result: <b>%1</b>").arg(result));
}
