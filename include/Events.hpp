#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <map>

namespace sg {

/**
 * @brief base class for all signal events
 */
class EventBase {
public:
	EventBase() = default;
	~EventBase() = default;
};

/**
 * @brief node event type use for all node lifecyle events
 */
class NodeEvent : public EventBase {
public:
	NodeEvent(NodeRef& target) : mTarget(target) {}
	
protected:
	NodeRef mTarget;
};

}
