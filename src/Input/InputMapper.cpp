#include "InputMapper.h"
#include "InputContext.h"

#include <iostream>

using namespace InputMapping;

InputMapper::InputMapper()
{
}

InputMapper::~InputMapper()
{
	for (std::map<std::string, InputContext*>::iterator iter = InputContexts.begin(); iter != InputContexts.end(); ++iter)
		delete iter->second;
}

void InputMapper::Clear()
{
	// do not clear states
	CurrentMappedInput.Actions.clear();
	CurrentMappedInput.Ranges.clear();
}

void InputMapper::SetRawButtonState(RawInputButton button, bool pressed, bool previouslyPressed)
{
	Action action;
	State state;

	// if button pressed this frame, map to action and insert action
	if (pressed && !previouslyPressed)
	{
		if (MapButtonToAction(button, action))
		{
			CurrentMappedInput.Actions.insert(action);
			return;
		}
	}

	// if button pressed, insert to state
	if (pressed)
	{
		if (MapButtonToState(button, state))
		{
			CurrentMappedInput.States.insert(state);
			return;
		}
	}

	// consume action mapped to button
	MapAndConsumeButton(button);
}

void InputMapper::SetRawAxisValue(RawInputAxis axis, double value)
{
	for (std::list<InputContext*>::const_iterator iter = ActiveContexts.begin(); iter != ActiveContexts.end(); ++iter)
	{
		const InputContext* context = *iter;

		Range range;
		if (context->MapAxisToRange(axis, range))
		{
			CurrentMappedInput.Ranges[range] = context->GetConversions().Convert(range, value * context->GetSensitivity(range));
			break;
		}
	}
}

void InputMapper::Dispatch() const
{
	// copy current input and call all callbacks
	MappedInput input = CurrentMappedInput;
	for (std::multimap<int, InputCallback>::const_iterator iter = CallbackTable.begin(); iter != CallbackTable.end(); ++iter)
		(*iter->second)(input);
}

void InputMapper::AddCallback(InputCallback callback, int priority)
{
	CallbackTable.insert(std::make_pair(priority, callback));
}

// push an active input context onto the stack
void InputMapper::PushContext(const std::string& name)
{
	std::map<std::string, InputContext*>::iterator iter = InputContexts.find(name);
	if (iter == InputContexts.end())
		throw std::exception("Invalid input context pushed!");

	ActiveContexts.push_front(iter->second);
}

// pop the current input context from the stack
void InputMapper::PopContext()
{
	if (ActiveContexts.empty())
		throw std::exception("Cannot pop input context, no contexts active!");
	
	ActiveContexts.pop_front();
}

// map button to an action in current active context(s)
bool InputMapper::MapButtonToAction(RawInputButton button, Action& action) const
{
	for (std::list<InputContext*>::const_iterator iter = ActiveContexts.begin(); iter != ActiveContexts.end(); ++iter)
	{
		const InputContext* context = *iter;

		if (context->MapButtonToAction(button, action))
			return true;
	}

	return false;
}

// map button to a state in current active context(s)
bool InputMapper::MapButtonToState(RawInputButton button, State& state) const
{
	for (std::list<InputContext*>::const_iterator iter = ActiveContexts.begin(); iter != ActiveContexts.end(); ++iter)
	{
		const InputContext* context = *iter;

		if (context->MapButtonToState(button, state))
			return true;
	}

	return false;
}

// consume all input mapped to a given button
void InputMapper::MapAndConsumeButton(RawInputButton button)
{
	Action action;
	State state;

	if (MapButtonToAction(button, action))
		CurrentMappedInput.ConsumeAction(action);

	if (MapButtonToState(button, state))
		CurrentMappedInput.ConsumeState(state);
}
