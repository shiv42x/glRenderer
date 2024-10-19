#include "RangeConverter.h"

#include <fstream>

using namespace InputMapping;

// construct converter and load provided conversion table
RangeConverter::RangeConverter(std::wifstream& infile)
{
	if (!infile)
		throw std::exception("Invalid file provided to RangeConverter constructor");

	/*
	unsigned int numConversions;
	for (unsigned int i = 0; i < numConversions; i++)
	{
		Range range;
		Converter converter;

		range = static_cast<Range> read range
		converter.MinInput = read min input
		converter.MaxInput = read max input
		converter.MinOutput = read min output
		converter.MaxOutput = read max output

		some error handling
		if MaxInput < MinInput or MaxOutput < MinInput
		throw invalid input range conversion

		ConversionMap.insert(std::make_pair(range, converter));
	}
	*/
}