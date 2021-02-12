#include <algorithm>

#include <functional>

#include "cinder/gl/gl.h"

#include "Node.h"

using namespace cg;
using namespace ci;
using namespace std;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	Produce test cases for deletions! definitely not working as expected..
// TODO:	Include methods that will report aggregate information about all children (total width, height, etc)
//
///////////////////////////////////////////////////////////////////////////

Node::Node(const string& name, const bool active)
:	mName(name), mIsActive(active)
{
}

Node::~Node()
{
	mParent.reset();
	removeChildren();
}

void Node::setParent(NodeRef node)
{
	bool dispatchAddedToScene = !hasParent();
	
	mParent = NodeWeakRef(node);
	
	if (dispatchAddedToScene) addedToScene();
}

bool Node::hasChildren() const
{
	return !mChildren.empty();
}

bool Node::hasChild(NodeRef node) const
{
	typename NodeDeque::const_iterator itr = std::find(mChildren.begin(), mChildren.end(), node);
	return itr != mChildren.end();
}

NodeRef Node::getChildByName(const string& name) const
{
	NodeRef node;
	
	typename NodeDeque::const_iterator itr;
	for (itr = mChildren.begin(); itr != mChildren.end(); itr++)
	{
		if (name == (*itr)->getName()) {
			node = (*itr);
			break;
		}
	}
	
	return node;
}

bool Node::addChild(NodeRef node)
{
	if (hasChild(node) || node.get() == this || node.get() == NULL) {
		return false;
	}
	
	// remove child from current parent
	std::shared_ptr<Node> parent = node->getParent();
	if (parent) parent->removeChild(node);

	// add to children
	mChildren.push_back(node);

	// set parent
	node->setParent( shared_from_this() );
	
	// dispatch addedToScene
	node->addedToScene();
	
	return true;
}

bool Node::removeChild(NodeRef node)
{
	typename NodeDeque::iterator itr = std::find(mChildren.begin(), mChildren.end(), node);
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

void Node::removeChildren()
{
	typename NodeDeque::iterator itr;
	for (itr = mChildren.begin(); itr != mChildren.end();)
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

void Node::removeFromParent()
{
	NodeRef node = mParent.lock();
	if (node) node->removeChild( shared_from_this() );
}

void Node::moveToTop()
{
	NodeRef parent = getParent();
	if (parent) parent->moveToTop( shared_from_this() );
}

void Node::moveToTop(NodeRef node)
{
	// remove from list
	typename NodeDeque::iterator itr = std::find(mChildren.begin(), mChildren.end(), node);
	if (itr == mChildren.end()) return;

	mChildren.erase(itr);

	// add to end of list
	mChildren.push_back(node);
}

bool Node::isOnTop() const
{	
	NodeRef parent = getParent();
	if (parent) return parent->isOnTop( std::const_pointer_cast<cg::Node>(shared_from_this()) );
	else return false;
}

bool Node::isOnTop(const NodeRef node) const
{
	if (mChildren.empty()) return false;
	if (mChildren.back() == node) return true;
	return false;
}

bool Node::isOnBottom() const
{
	NodeRef parent = getParent();
	if (parent) return parent->isOnBottom( std::const_pointer_cast<cg::Node>(shared_from_this()) );
	else return false;
}

bool Node::isOnBottom(const NodeRef node) const
{
	if (mChildren.empty()) return false;
	if (mChildren.front() == node) return true;
	return false;
}

void Node::moveToBottom()
{
	NodeRef parent = getParent();
	if (parent) parent->moveToBottom( shared_from_this() );
}

void Node::moveToBottom(NodeRef node)
{
	// remove from list
	typename NodeDeque::iterator itr = std::find(mChildren.begin(), mChildren.end(), node);
	if (itr == mChildren.end()) return;

	mChildren.erase(itr);

	// add to start of list
	mChildren.push_front(node);
}

void Node::deepSetup()
{
	setup();

	typename NodeDeque::iterator itr;
	for (itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		(*itr)->deepSetup();
	}
}

void Node::deepUpdate(double elapsed)
{
	// let derived class perform animation 
	update(elapsed);

	// update this node's children
	typename NodeDeque::iterator itr;
	for (itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		(*itr)->deepUpdate(elapsed);
	}
}
