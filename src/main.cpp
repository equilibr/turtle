#include "main.h"

#include <functional>
#include <vector>
#include <utility>

void Main(ThreadedBrain & brain)
{
	using Algorithm = std::function<void(ThreadedBrain&)>;

	std::vector<std::pair<Algorithm, QString>> algorithms =
	{
		{Draw, "Draw"},
		{Follow, "Follow"},
		{Adder, "Adder"},
	};

	QString algorithmList;
	for (size_t i = 0; i < algorithms.size(); ++i)
	{
		algorithmList.append(QString::number(i) + ": ");
		algorithmList.append(algorithms[i].second);
		algorithmList.append("\n");
	}

	static int lastSelection = 0;
	bool ok;
	lastSelection = brain.getInteger("Select an algorithm", algorithmList, lastSelection, &ok);
	if (!ok)
		return;

	const size_t algorithm = static_cast<size_t>(lastSelection);

	if (algorithm >= algorithms.size())
	{
		brain.log("<b>Invalid selection!</b>");
		return;
	}

	brain.log("Executing algorithm: <b>" + algorithms[algorithm].second + "</b>");
	algorithms[algorithm].first(brain);
}
