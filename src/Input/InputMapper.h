#pragma once

#include "Constants/RawInputs.h"
#include "Constants/InputConstants.h"
#include "InputContext.h"

#include <map>
#include <set>
#include <list>
#include <string>

namespace InputMapping
{
	struct MappedInput
	{
		std::set<Action> Actions;
		std::set<State> States;
		std::map<Range, double> Ranges;

		void ConsumeAction(Action action) { Actions.erase(action); }
		void ConsumeState(State state) { States.erase(state); }
		void ConsumeRange(Range range)
		{
			std::map<Range, double>::iterator it = Ranges.find(range);
			if (it != Ranges.end())
				Ranges.erase(it);
		}
	};

	typedef void (*InputCallback)(MappedInput& input);

	class InputMapper
	{
	public:
		InputMapper();
		~InputMapper();


	// raw input
	public:
		void Clear();
		void SetRawButtonState(RawInputButton button, bool pressed, bool previouslyPressed);
		void SetRawAxisValue(RawInputAxis axis, double value);

	// dispatching input
	public:
		void Dispatch() const;

	// callback registration
	public:
		void AddCallback(InputCallback callback, int priority);

	// context management
	public:
		void PushContext(const std::string& name);
		void PopContext();

	// internal helpers
	private:
		bool MapButtonToAction(RawInputButton button, Action& action) const;
		bool MapButtonToState(RawInputButton button, State& state) const;
		void MapAndConsumeButton(RawInputButton button);

	// internal tracking
	private:
		std::map<std::string, InputContext*> InputContexts;
		std::list<InputContext*> ActiveContexts;

		// priority -> callback
		std::multimap<int, InputCallback> CallbackTable;

		MappedInput CurrentMappedInput;
	};
}