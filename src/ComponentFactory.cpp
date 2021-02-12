#include <algorithm>

#include "ComponentFactory.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace scene;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	Produce test cases
//
///////////////////////////////////////////////////////////////////////////

ComponentFactory::ComponentFactory()
{
}

ComponentFactory::~ComponentFactory()
{
}

void ComponentFactory::setup()
{
	// is this needed?
}

void ComponentFactory::update(double elapsed)
{
	// update all components
}

ComponentRef ComponentFactory::instantiate(const std::string& name)
{
	ComponentRef comp(new ComponentBase(name));
	
	mComponents.push_back(comp);
	
	return comp;
}

bool ComponentFactory::addComponent(ComponentRef comp)
{
	if (!comp) return false;
	
	mComponents.push_back(comp);
	
	return true;
}

bool ComponentFactory::removeComponent(ComponentRef comp)
{
	if (!comp) return false;
	
	int s = mComponents.size();
	remove(mComponents.begin(), mComponents.end(), comp);
	return s != mComponents.size();
}

ComponentRef ComponentFactory::getComponentByName(const std::string& name) const
{
}

bool ComponentFactory::clearComponents()
{
	mComponents.clear();
}
