#pragma once

#include <string>
#include <iostream>
#include <memory>

#include "SceneObject.h"

namespace scene {

class ComponentBase;
typedef std::shared_ptr<ComponentBase> ComponentRef;			//!< A shared pointer to a Component instance
typedef std::shared_ptr<const ComponentBase> ComponentConstRef;	//!< A shared pointer to a constant Component instance
typedef std::weak_ptr<ComponentBase> ComponentWeakRef;			//!< A weak pointer to a Component instance

/**
 * @brief Component is an abstract base class for components added to the scenegraph
 *
 */
class ComponentBase : public scene::SceneObject {
public:
	virtual ~ComponentBase() {}
	
	/**
	 * Invoked by the factory that instantiates the Component instance.
	 * All components created using a factory have a setup method for initialization.
	 */
	virtual void setup() {};
	
	/**
	 * Invoked by the factory that instantiates the Component instance.
	 * Each factory uses this update step to apply changes and manage component lifecycle.
	 */
	virtual void update( double elapsed ) {};
	
	// stream logging support
	friend std::ostream& operator<<(std::ostream& lhs, const ComponentBase& rhs) {
		return lhs << "[Component name=" << rhs.mName << "]";
	}
	
protected:
	friend class ComponentFactory;
	
	ComponentBase(const std::string& name = "Component") : scene::SceneObject(name) {};
};
	
}
