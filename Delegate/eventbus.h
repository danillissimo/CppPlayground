#pragma once

#include <unordered_set>

#include "delegate.h"


template <typename ReturnType, typename... FunctionArguments>
class EventBusClient
{
protected:
	typedef Delegate<ReturnType, FunctionArguments...> DelegateType;
	typedef
	std::unordered_set<DelegateType, typename DelegateType::hash, typename DelegateType::equal_to>
	ContainerType;

private:
	ContainerType listeners;

public:
	const ContainerType & getListeners( ) const { return listeners; }

	EventBusClient( ) { listeners = ContainerType( ); }
	EventBusClient(size_t reserve) { listeners = ContainerType(reserve); }

	void subscribe(DelegateType & delegate) { listeners.insert(delegate); }
	void subscribe(DelegateType && delegate) { listeners.insert(delegate); }
	void unsubscribe(DelegateType & delegate) { listeners.erase(delegate); }

	size_t getNumListeners( ) const { return listeners.size( ); }
	bool   hasListeners( ) const { return listeners.size( ) > 0; }
};


template <typename ReturnType, typename... FunctionArguments>
class EventBus : public EventBusClient<ReturnType, FunctionArguments...>
{
public:
	void fireEvent(FunctionArguments ...args) const
	{
		auto listeners = this->getListeners( );
		for (auto i = listeners.begin( ); i != listeners.end( ); i++)
		{
			i->invoke(args...);
		}
	}

	EventBusClient<ReturnType, FunctionArguments...> & operator!( ) const
	{
		return static_cast<EventBusClient<ReturnType, FunctionArguments...> &>(*this);
	}
};
