#include "InputContext.h"

#include <fstream>

using namespace InputMapping;

InputContext::InputContext(const std::string& filename)
{

}

InputContext::~InputContext()
{
	delete Conversions;
}

// attempt to map raw button to action
bool InputContext::MapButtonToAction(RawInputButton button, Action& action) const
{
	std::map<RawInputButton, Action>::const_iterator iter = ActionMap.find(button);
	if (iter == ActionMap.end())
		return false;

	// mapped action
	action = iter->second;
	return true;
}

// attempt to map raw button to state
bool InputContext::MapButtonToState(RawInputButton button, State& state) const
{
	std::map<RawInputButton, State>::const_iterator iter = StateMap.find(button);
	if (iter == StateMap.end())
		return false;

	// mapped state
	state = iter->second;
	return true;
}

// attempt to map raw axis to range
bool InputContext::MapAxisToRange(RawInputAxis axis, Range& range) const
{
	std::map<RawInputAxis, Range>::const_iterator iter = RangeMap.find(axis);
	if (iter == RangeMap.end())
		return false;

	// mapped range
	range = iter->second;
	return true;
}

// get sensitivity for range
double InputContext::GetSensitivity(Range range) const
{
	std::map<Range, double>::const_iterator iter = SensMap.find(range);
	if (iter == SensMap.end())
		return 1.0;

	// sensitivity
	return iter->second;
}