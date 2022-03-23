#pragma once

#include <string>
#include <vector>
#include <memory>

#include "ComponentBase.hpp"

namespace sg {

class ComponentFactory;
typedef std::shared_ptr<ComponentFactory> ComponentFactoryRef;				//!< A shared pointer to a ComponentFactory instance
typedef std::shared_ptr<const ComponentFactory> ComponentFactoryConstRef;	//!< A shared pointer to a constant ComponentFactory instance
typedef std::weak_ptr<ComponentFactory> ComponentFactoryWeakRef;			//!< A weak pointer to a ComponentFactory instance

/**
 * @brief ComponentFactory is an abstract base class for creating components
 *
 * The only way that components should be created is through a factory class.
 * This allows for simple, centralized management and upkeep for each component type.
 *
 * @see sg::Component
 */
class ComponentFactory : public sg::SceneObject {
public:
	ComponentFactory();
	virtual ~ComponentFactory();
	
	virtual void setup();
	
	virtual void update( double elapsed );
	
//	ComponentRef create(const std::string& name);
	
	static ComponentFactoryWeakRef get()
	{
		if (nullptr == sInstance.get())
			sInstance = std::make_shared<ComponentFactory>();

		return sInstance;
	}
	
	virtual ComponentRef create(const std::string& name) { return ComponentRef(); }
	
protected:
	bool addComponent(ComponentRef comp);
	bool removeComponent(ComponentRef comp);
	ComponentRef getComponentByName(const std::string& name) const;
	void clearComponents();
	
private:
	static ComponentFactoryRef sInstance;
	
	std::vector<ComponentRef> mComponents;
	
};
	
}
