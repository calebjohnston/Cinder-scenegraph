#pragma once

#include <string>
#include <iostream>
#include <queue>
#include <deque>
#include <memory>

#include "cinder/app/App.h"
#include "cinder/AxisAlignedBox.h"

#include "SceneObject.h"

namespace scene {

class NodeBase;
typedef std::shared_ptr<NodeBase> NodeRef;				//!< A shared pointer to a Node2d instance
typedef std::shared_ptr<const NodeBase> NodeConstRef;	//!< A shared pointer to a constant Node2d instance
typedef std::weak_ptr<NodeBase> NodeWeakRef;			//!< A weak pointer to a Node2d instance
typedef std::deque<NodeRef> NodeDeque;					//!< A deque of shared pointers to Node2d instances

/**
 * @brief Abstract base class for all scene graph entities
 *
 * NodeBase abstract base class. It supports a hierarchical scene graph
 * system for coordinate transformations and rendering. Each concrete
 * node type can be parsed from an input XML node. Each node type 
 * can also be serialized to an XML node.
 */
class NodeBase : public scene::SceneObject {
public:
	/**
	 * Virtual destructor destroys children
	 */
	virtual ~NodeBase();
	
	//! returns true if this node has a parent assigned, false otherwise
	bool hasParent() const { return mParent.lock().get() != nullptr; }
	
	//! sets the node's parent node (using weak reference to avoid objects not getting destroyed)
	void setParent(NodeRef node);
	
	//! returns the node's parent node 
	NodeRef	getParent() const { return mParent.lock(); }
	
	//! returns the node's parent node (provide a templated function for easier down-casting of nodes)
	template<class Y>
	std::shared_ptr<Y> getParent() const
	{
		return std::dynamic_pointer_cast<Y>(mParent.lock());
	}
	
	//! returns a node higher up in the hierarchy of the desired type, if any
	template<class Y>
	std::shared_ptr<Y> getDeepParent()
	{
		std::shared_ptr<Y> node = std::dynamic_pointer_cast<Y>(mParent.lock());
		if (node) {
			return node;
		}
		else if (mParent.lock()){
			std::shared_ptr<Y> parent = mParent.lock()->template getDeepParent<Y>();
			return parent;
		}
		else return node;
	}

	//! returns wether this node has any children
	bool hasChildren() const;
	
	//! returns children with read access only
	const NodeDeque& getChildren() const { return mChildren; }
	
	//! returns children with read access only
	NodeDeque& getChildren() { return mChildren; }
	
	//! returns the number of children that this node contains
	size_t getChildCount() const { return mChildren.size(); }
	
	//! returns wether this node has a specific child
	bool hasChild(NodeRef node) const;
	
	//! returns a NodeRef to a child using it's name if it exists, otherwise NULL NodeRef
	NodeRef getChildByName(const std::string& name) const;
	
	//! adds a child to this node if it wasn't already a child of this node
	bool addChild(NodeRef node);
	
	//! removes a specific child from this node
	bool removeChild(NodeRef node);
	
	//! removes all children of this node
	void removeChildren();
	
	//! puts a specific child on top of all other children of this node
	void moveToTop(NodeRef node);
	
	//! returns wether a specific child is on top of all other children
	bool isOnTop(NodeConstRef node) const;
	
	//! returns wether a specific child is beneath all other children
	bool isOnBottom(NodeConstRef node) const;
	
	//! puts a specific child below all other children of this node
	void moveToBottom(NodeRef node);

	// child functions
	//! removes this node from its parent
	void removeFromParent();
	
	//! puts this node on top of all its siblings
	void moveToTop();
	
	//! returns wether this node is on top of all its siblings
	bool isOnTop() const;
	
	//! returns wether this node is beneath all its siblings
	bool isOnBottom() const;
	
	//! puts this node below all its siblings
	void moveToBottom();

	//! enables or disables visibility of this node (inactive nodes are not drawn and can not receive events, but they still receive updates)
	virtual void setActive(bool active = true) { mIsActive = active; }
	
	//! returns wether this node is active
	virtual bool isActive() const { return mIsActive; }

	//! calls the setup() function of this node and all its decendants
	virtual void deepSetup();
	//! calls the update() function of this node and all its decendants
	virtual void deepUpdate(double elapsed);
	//! calls the draw() function of this node and all its decendants
	virtual void deepDraw() = 0;

	// base class has no impl for standard event loop
	virtual void setup() { /* no-op */ }
	virtual void update(double elapsed) { /* no-op */ }
	virtual void draw() { /* no-op */ }
	virtual void addedToScene() { /* no-op */ }
	virtual void removedFromScene() { /* no-op */ }
		
protected:
	/**
	 * Default constructor is protected
	 *
	 * @param name used to identify the node for development purposes
	 * @param active flag used to determine if the node will be active upon instantiation
	 * @param interactive flag used to determine if the node will accept and transmit mouse and key events
	 */
	NodeBase(const std::string& name = "", const bool active = true);
	
	bool			mIsActive;		//!< visibility flag when drawing the node
	NodeWeakRef		mParent;		//!< std::weak_ptr<class Node> parent
	NodeDeque		mChildren;		//!< std::deque<std::shared_ptr<class Node> > children

	//! function that is called right before drawing this node
	virtual void pre_draw() {}
		
	//! function that is called right after drawing this node
	virtual void post_draw() {}

	//! required transform() function to compose the transformation matrix
	virtual void transform() = 0;
	
public:
	/**
	 * @brief Iterator type for the Node tree data structure
	 *
	 * This iterator is designed to support both breadth-first and depth-first
	 * traversal of the Node's children. It is also defined using a template
	 * so that it can be specialized to const and non-const iterator types.
	 * Breadth-first implementation utilizes a queue. Depth-first uses a stack.
	 *
	 * The space complexity for depth first search: O(c*h)
	 * while the space complexity for breadth first search: O(c^h) where
	 *    c is the number of children in each node; and
	 *    h is the height of the tree.
	 */
	template<typename T>
	class Iterator {
	public:
		/** Constructor initializes the iterator from the specified root node */
		Iterator(T root, const bool depth_first) : mDepthFirst(depth_first)
		{
			if (depth_first) mStack.push(root);
			else mQueue.push(root);
		}
		/** Destructor clears internals */
		~Iterator()
		{
			while (!mStack.empty()) mStack.pop();
			while (!mQueue.empty()) mQueue.pop();
		}
		
		//! Returns true if more nodes exist
		bool hasNext() const {
			return mDepthFirst? !mStack.empty(): !mQueue.empty();
		}
		
		/**
		 * Performs one step in the iteration process. The iteration will
		 * grab the next node from the container, add it's children to
		 * the container and then its returned.
		 */
		T next()
		{
			T node;
			
			if (mDepthFirst) {
				if (!mStack.empty()) {
					node = mStack.top();
					mStack.pop();
					if (node->hasChildren()) {
						NodeDeque children = node->getChildren();
						NodeDeque::iterator iter;
						for (iter = children.begin(); iter != children.end(); iter++) {
							mStack.push(*iter);
						}
					}
				}
			}
			else {
				if (!mQueue.empty()) {
					node = mQueue.front();
					mQueue.pop();
					if (node->hasChildren()) {
						NodeDeque children = node->getChildren();
						NodeDeque::iterator iter;
						for (iter = children.begin(); iter != children.end(); iter++) {
							mQueue.push(*iter);
						}
					}
				}
			}
			
			return node;
		}
		
		/**
		 * Template next implementation simply uses the dynamic cast to convert the
		 * returned type to the desired type. For instance a NodeRef to a NodeMeshRef.
		 */
		template<class Y>
		std::shared_ptr<Y> next() { return std::dynamic_pointer_cast<Y>(next()); }
		
	private:
		const bool mDepthFirst;	//! Stores the flag for choosing the type of traversal
		std::stack<T> mStack;	//! Container used for depth-first traversal
		std::queue<T> mQueue;	//! Container used for breadth-first traversal
	};
	
	typedef Iterator<NodeRef> Iter;				//!< The typical non-const iterator type declaration
	typedef Iterator<NodeConstRef> ConstIter;	//!< The const iterator type declaration
	
	//! Returns an iterator. Defaults to depth first
	Iter getIter(const bool depth_first = true) { return Iter(shared_from_base<NodeBase>(), depth_first); }
	//! Returns a depth first iterator
	Iter getDepthFirstIter() { return Iter(shared_from_base<NodeBase>(), true); }
	//! Returns a breadth first iterator
	Iter getBreadthFirstIter() { return Iter(shared_from_base<NodeBase>(), false); }
	
	//! Returns a constant iterator. Defaults to depth first
	ConstIter getConstIter(const bool depth_first = true) const { return ConstIter(shared_from_base<NodeBase>(), depth_first); }
	//! Returns a depth first constant iterator
	ConstIter getDepthFirstConstIter() const { return ConstIter(shared_from_base<NodeBase>(), true); }
	//! Returns a breadth first constant iterator
	ConstIter getBreadthFirstConstIter() const { return ConstIter(shared_from_base<NodeBase>(), false); }
	
};

}
