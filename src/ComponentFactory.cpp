#include <algorithm>

#include "ComponentFactory.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace sg;

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

//ComponentRef ComponentFactory::create(const std::string& name)
//{
//	ComponentRef comp(new ComponentBase(name));
//
//	addComponent(comp);
//
//	return comp;
//}

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
	auto match = find_if(begin(mComponents), end(mComponents), [&](ComponentRef c){ return c->getName() == name; });
	if (match != end(mComponents))
		return *match;
	else
		return ComponentRef();
}

void ComponentFactory::clearComponents()
{
	mComponents.clear();
}
