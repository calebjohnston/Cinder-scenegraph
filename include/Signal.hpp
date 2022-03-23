#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <map>

#include "Events.hpp"

namespace sg {

template <typename E>
class Signal {
public:
	Signal()  = default;
	~Signal() = default;

	//! Copy and Assignment c'tors creates fresh object
	Signal(const Signal& other) {}
	Signal& operator=(const Signal& other)
	{
		if (this != &other)
			clearListeners();
		
		return *this;
	}

	//! Move c'tors work as expected
	Signal(Signal&& other) noexcept : mSlots(std::move(other.mSlots)), mCurrentId(other.mCurrentId) {}
	Signal& operator=(Signal&& other) noexcept {
		if (this != &other) {
			mSlots     = std::move(other.mSlots);
			mCurrentId = other.mCurrentId;
		}

		return *this;
	}

	//! Connects a std::function to the signal. The returned value can be used to disconnect the function again.
	int addListener(const std::function<void(E)>& slot)
	{
		mSlots.insert(std::make_pair(++mCurrentId, slot));
		return mCurrentId;
	}

	//! Convenience method to connect a member function of an object to this Signal.
	template<typename T>
	int addListener(T *inst, void (T::*func)(E))
	{
		return addListener([=](E event) {
			(inst->*func)(event);
		});
	}

	//! Convenience method to connect a const member function of an object to this Signal.
	template<typename T>
	int addListener(T *inst, const void (T::*func)(E))
	{
		return addListener([=](E event) {
			(inst->*func)(event);
		});
	}

	//! Disconnects a listener function
	bool removeListener(int id)
	{
		size_t removed = mSlots.erase(id);
		return removed > 0;
	}

	//! Removes all previously connected listener functions
	void clearListeners()
	{
		mSlots.clear();
	}

	//! Dispatches an event to all listeners
	void dispatch(E event)
	{
		for (auto const& it : mSlots) {
			it.second(event);
		}
	}

private:
	std::map<int, std::function<void(E)>> mSlots;
	int mCurrentId {0};
};

}
