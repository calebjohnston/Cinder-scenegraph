#include <algorithm>

#include "TransformComponent.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace sg;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	Produce test cases
//
///////////////////////////////////////////////////////////////////////////

TransformComponent::TransformComponent(const std::string& name, const ci::mat4& transform)
:	sg::ComponentBase(name), mTransform(transform)
{
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::setup()
{
}

void TransformComponent::update( double elapsed )
{
}


///////////////////////////////////////////////////////////////////////////


TransformFactory::TransformFactory()
{
}

TransformFactory::~TransformFactory()
{
}

void TransformFactory::setup()
{
	ComponentFactory::setup();
}

void TransformFactory::update( double elapsed )
{
	ComponentFactory::update(elapsed);
}

ComponentRef TransformFactory::create(const std::string& name, const ci::mat4& transform)
{
	TransformRef comp(new TransformComponent(name, transform));

	addComponent(comp);
	
	return comp;
}

ComponentRef TransformFactory::create(const std::string& name)
{
	TransformRef comp(new TransformComponent(name));

	addComponent(comp);
	
	return comp;
}
