#include "RectTransformComponent.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace sg;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	Produce test cases
//
///////////////////////////////////////////////////////////////////////////

RectTransformComponent::RectTransformComponent(const std::string& name, const ci::mat3& transform) : sg::ComponentBase(name), mTransform(transform)
{
}

RectTransformComponent::~RectTransformComponent()
{
}

void RectTransformComponent::setup()
{
}

void RectTransformComponent::update( double elapsed )
{
}

///////////////////////////////////////////////////////////////////////////


RectTransformFactory::RectTransformFactory()
{
}

RectTransformFactory::~RectTransformFactory()
{
}

void RectTransformFactory::setup()
{
	ComponentFactory::setup();
}

void RectTransformFactory::update( double elapsed )
{
	ComponentFactory::update(elapsed);
}

ComponentRef RectTransformFactory::create(const std::string& name, const ci::mat3& transform)
{
	RectTransformRef comp(new RectTransformComponent(name, transform));

	addComponent(comp);
	
	return comp;
}

ComponentRef RectTransformFactory::create(const std::string& name)
{
	RectTransformRef comp(new RectTransformComponent(name));

	addComponent(comp);
	
	return comp;
}
