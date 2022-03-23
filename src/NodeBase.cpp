#include <algorithm>

#include <functional>

#include "cinder/gl/gl.h"

#include "NodeBase.h"

using namespace sg;
using namespace ci;
using namespace std;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	Produce test cases for deletions! definitely not working as expected..
// TODO:	Include methods that will report aggregate information about all children (total width, height, etc)
//
///////////////////////////////////////////////////////////////////////////

NodeBase::NodeBase(const string& name, const bool active)
:	SceneObject(name), mIsActive(active)
{
}

NodeBase::~NodeBase()
{
	mParent.reset();
	removeChildren();
}

void NodeBase::setParent(NodeRef node)
{
	bool dispatchAddedToScene = !hasParent();
	
	mParent = NodeWeakRef(node);
	
	if (dispatchAddedToScene) addedToScene();
}

bool NodeBase::hasChildren() const
{
	return !mChildren.empty();
}

bool NodeBase::hasChild(NodeRef node) const
{
	auto itr = std::find(mChildren.begin(), mChildren.end(), node);
	return itr != mChildren.end();
}

NodeRef NodeBase::getChildByName(const string& name) const
{
	NodeRef node;
	
	for (auto itr = mChildren.begin(); itr != mChildren.end(); itr++)
	{
		if (name == (*itr)->getName()) {
			node = (*itr);
			break;
		}
	}
	
	return node;
}

bool NodeBase::addChild(NodeRef node)
{
	if (hasChild(node) || node.get() == this || node.get() == nullptr) {
		return false;
	}
	
	// remove child from current parent
	std::shared_ptr<NodeBase> parent = node->getParent();
	if (parent) parent->removeChild(node);

	// add to children
	mChildren.push_back(node);

	// set parent
	node->setParent( shared_from_base<NodeBase>() );
	
	// dispatch addedToScene
	node->addedToScene();
	
	return true;
}

bool NodeBase::removeChild(NodeRef node)
{
	auto itr = std::find(mChildren.begin(), mChildren.end(), node);
	if (itr != mChildren.end())
	{
		// reset parent
		(*itr)->setParent( NodeRef() );
		(*itr).reset();

		// remove from children
		mChildren.erase(itr);
		
		// dispatch removedFromScene
		node->removedFromScene();
		
		return true;
	}
	else return false;
}

void NodeBase::removeChildren()
{
	for (auto itr = mChildren.begin(); itr != mChildren.end();)
	{
		// reset parent
//		(*itr)->setParent( NodeRef() );
		(*itr).reset();

		// remove from children
		itr = mChildren.erase(itr);
		
		// dispatch removedFromScene
		(*itr)->removedFromScene();
	}
}

void NodeBase::removeFromParent()
{
	NodeRef node = mParent.lock();
	if (node) node->removeChild( shared_from_base<NodeBase>() );
}

void NodeBase::moveToTop()
{
	NodeRef parent = getParent();
	if (parent) parent->moveToTop( shared_from_base<NodeBase>() );
}

void NodeBase::moveToTop(NodeRef node)
{
	// remove from list
	auto itr = std::find(mChildren.begin(), mChildren.end(), node);
	if (itr == mChildren.end()) return;

	mChildren.erase(itr);

	// add to end of list
	mChildren.push_back(node);
}

bool NodeBase::isOnTop() const
{	
	NodeRef parent = getParent();
	if (parent)
		return parent->isOnTop( shared_from_base<NodeBase>() );
	else
		return false;
}

bool NodeBase::isOnTop(NodeConstRef node) const
{
	if (mChildren.empty())
		return false;
	if (mChildren.back() == node)
		return true;
	return false;
}

bool NodeBase::isOnBottom() const
{
	NodeRef parent = getParent();
	if (parent)
		return parent->isOnBottom( shared_from_base<NodeBase>() );
	else
		return false;
}

bool NodeBase::isOnBottom(NodeConstRef node) const
{
	if (mChildren.empty())
		return false;
	if (mChildren.front() == node)
		return true;
	return false;
}

void NodeBase::moveToBottom()
{
	NodeRef parent = getParent();
	if (parent)
		parent->moveToBottom( shared_from_base<NodeBase>() );
}

void NodeBase::moveToBottom(NodeRef node)
{
	// remove from list
	auto itr = std::find(mChildren.begin(), mChildren.end(), node);
	if (itr == mChildren.end()) return;

	mChildren.erase(itr);

	// add to start of list
	mChildren.push_front(node);
}

void NodeBase::deepSetup()
{
	setup();

	for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		(*itr)->deepSetup();
	}
}

void NodeBase::deepUpdate(double elapsed)
{
	// let derived class perform animation 
	update(elapsed);

	// update this node's children
	for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		(*itr)->deepUpdate(elapsed);
	}
}
