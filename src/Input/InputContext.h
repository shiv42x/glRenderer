#pragma once

#include "Constants/RawInputs.h"
#include "Constants/InputConstants.h"
#include "RangeConverter.h"

#include <map>
#include <string>

namespace InputMapping
{
	class InputContext
	{
	public:
		InputContext(const std::string& filename);
		~InputContext();

	public:
		bool MapButtonToAction(RawInputButton button, Action& action) const;
		bool MapButtonToState(RawInputButton button, State& state) const;
		bool MapAxisToRange(RawInputAxis axis, Range& range) const;

		double GetSensitivity(Range range) const;

		const RangeConverter& GetConversions() const { return *Conversions; }
	
	private:
		std::map<RawInputButton, Action> ActionMap;
		std::map<RawInputButton, State> StateMap;
		std::map<RawInputAxis, Range> RangeMap;

		std::map<Range, double> SensMap;
		RangeConverter* Conversions;
	};
}