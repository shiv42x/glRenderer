#pragma once

#include "Constants/InputConstants.h"

#include <map>
#include <fstream>

// converts raw input ranges to sensitivity-calibrated range values
namespace InputMapping
{
	class RangeConverter
	{
	private:
		struct Converter
		{
			double MinInput;
			double MaxInput;

			double MinOutput;
			double MaxOutput;

			template <typename RangeType>
			RangeType Convert(RangeType invalue) const
			{
				double v = static_cast<double>(invalue);

				// clamp to input ranges
				if (v < MinInput)
					v = MinInput;
				if (v > MaxInput)
					v = MaxInput;

				// linearly interpolate v to output range
				double interpolationFactor = (v - MinInput) / (MaxInput - MinInput);
				return static_cast<RangeType>((interpolationFactor * (MaxOutput - MinOutput)) + MinOutput);
			}
		};

	// internal type shortcuts
	private:
		typedef std::map<Range, Converter> ConversionMapT;

	// constructor
	public:
		RangeConverter(std::wifstream& infile);

	public:
		template <typename RangeType>
		RangeType Convert(Range rangeID, RangeType invalue) const
		{
			ConversionMapT::const_iterator iter = ConversionMap.find(rangeID);
			if (iter == ConversionMap.end())
				return invalue;

			return iter->second.Convert<RangeType>(invalue);
		}

	private:
		ConversionMapT ConversionMap;
	};
}



