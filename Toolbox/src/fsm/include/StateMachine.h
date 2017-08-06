/*
 * StateMachine.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

// standard
#include <functional>

// local
#include "FSMEvent.h"
#include "FSMEventTypes.h"

// project
#include "exceptionMacros.h"


#define FSM_TABLE_BEGIN(className) \
	using FSM_CLASS = className; \
	const FSM_CLASS::StateDefinition FSM_CLASS::StateTable[] = {

#define FSM_STATE(stateFunc) { #stateFunc, &FSM_CLASS :: stateFunc }

#define FSM_TABLE_END() };

#define FSM_DECL_TABLE() static const StateDefinition StateTable[]

#define FSM_TRANSITION(stateFunc) transition(&FSM_CLASS :: stateFunc)

#define FSM_CONSTRUCTOR(initialStateFunc) StateMachine(*this, &FSM_CLASS :: initialStateFunc, StateTable)

template <typename T>
class StateMachine
{
public:
	using StateFunction = std::function<void (T&, const FSMEvent&)>;
	using StateFunctionPtr = void (T::*)(const FSMEvent&);

	struct StateDefinition
	{
		const char* name;
		StateFunctionPtr state;
	};

	explicit StateMachine(T& instance, StateFunctionPtr initialState, const StateDefinition* const stateTable) : m_instance(instance),
			m_initialState(std::mem_fn(initialState)), m_currentState(nullptr),
			m_newState(nullptr), m_terminated(false)
	{
		TB_ASSERT(m_initialState);
	}

	void init()
	{
		m_currentState = m_initialState;
		emit(FSMEventTypes::EnterEvent);
	}

	void terminate()
	{
		TB_ASSERT(!m_terminated);
		TB_ASSERT(m_currentState);

		m_currentState(m_instance, FSMEvent(FSMEventTypes::ExitEvent));
		m_currentState = nullptr;
		m_terminated = true;
	}

	bool isTerminated() const
	{
		return m_terminated;
	}

	void emit(const FSMEvent::EventType_t type)
	{
		emit(FSMEvent(type));
	}

	void emit(const FSMEvent& event)
	{
		TB_ASSERT(!m_terminated);
		TB_ASSERT(!m_newState);
		TB_ASSERT(m_currentState);

		m_currentState(m_instance, event);

		while (m_newState)
		{
			auto pendingState = m_newState;
			m_newState = nullptr;

			m_currentState(m_instance, FSMEvent(FSMEventTypes::ExitEvent));

			// not allowed to change state in exit event
			TB_ASSERT(!m_newState);

			m_currentState = pendingState;

			m_currentState(m_instance, FSMEvent(FSMEventTypes::EnterEvent));
		}
	}

protected:

	void transition(StateFunctionPtr state)
	{
		m_newState = state;
	}

private:

	T& m_instance;
	StateFunction m_initialState;
	StateFunction m_currentState;
	StateFunction m_newState;

	bool m_terminated;
};

#endif /* STATEMACHINE_H_ */
