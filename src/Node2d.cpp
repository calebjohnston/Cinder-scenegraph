#include <algorithm>
#include <functional>
#include <limits>

#include "cinder/gl/gl.h"
#include "cinder/gl/wrapper.h"
#include "glm/gtx/vec_swizzle.hpp"

#include "Node2d.h"

using namespace cg;
using namespace ci;
using namespace std;

///////////////////////////////////////////////////////////////////////////
//
// TODO:
//
///////////////////////////////////////////////////////////////////////////

Node2dRef Node2d::create(const std::string& name, const bool active)
{
	return Node2dRef( new Node2d( name, active ) );
}

Node2d::Node2d(const std::string& name, const bool active)
:	Node(name, active), mSize(0), mPosition(0),
	mScale(1), mPivot(0), mTransform(1),
	mWorldTransform(1), mRotation(0)
{
	
}

Node2d::~Node2d()
{
	
}

void Node2d::deepTransform(const mat3& world)
{
	// update transform matrix by calling derived class's function
	transform();
	
	// calculate world transform matrix
	mWorldTransform = world * mTransform;
	
	// do the same for all children
//	NodeDeque::iterator itr;
	for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		std::shared_ptr<Node2d> child = std::dynamic_pointer_cast<Node2d>(*itr);
		child->deepTransform(mWorldTransform);
	}
}

void Node2d::deepDraw() {}
/*
void Node2d::deepDraw()
{
	if (!mIsActive) return;
	
	// let derived class know we are about to draw stuff
	pre_draw();
	
	// update transform matrix by calling derived class's function
	// (do it again here in case this node has no parent and thus wasn't updated)
	transform();
	
	// apply transform
	gl::pushModelView();
	
	// usual way to update modelview matrix
	Matrix44f local_transform(mTransform);
	gl::multModelView( local_transform );
	
	// draw this node by calling derived class
	draw();
	
	// draw this node's children
//	typename NodeDeque::iterator itr;
	for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		(*itr)->deepDraw();
	}
	
	// restore transform
	gl::popModelView();
	
	// let derived class know we are done drawing
	post_draw();
}
*/

void Node2d::transform()
{
	if (!mTransformIsDirty) return;
	
//	mTransform.setToIdentity();
//	mTransform.translate(mPosition);
//	mTransform.rotate(mRotation);
//	mTransform.scale(mScale);
//	mTransform.translate(-mPivot);
	
	mTransform = mat4(1);
	mTransform = glm::translate(mTransform, mPosition);
	mTransform = glm::rotate(mTransform, mRotation);
	mTransform = glm::translate(mTransform, mScale);
	mTransform = glm::translate(mTransform, -mPivot);
	mTransformIsDirty = false;
}

void Node2d::setRotation(const float radians, const bool use_degrees)
{
	mRotation = use_degrees? radians * 180.0/M_PI: radians;
	mTransformIsDirty = true;
}

vec2 Node2d::getPivotPercentage() const
{
	vec2 out;
	if (mSize.length() > 0.0f)
		out = mPivot / mSize;
	else
		out = vec2(0);
	
	return out;
}

void Node2d::setScreenRect(const Rectf& bounds)
{
	/** TODO: IMPLEMENT */
}

Rectf Node2d::getScreenRect(bool precise) const
{
	float max = std::numeric_limits<float>::max();
	float min = std::numeric_limits<float>::min();
	vec2 max_pt = vec3(max);
	vec2 min_pt = vec3(min);
	Rectf bounds(max_pt, min_pt);
	
	//NodeDeque::const_reverse_iterator itr;
	for (auto itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr) {
		Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
		if (child) {
			Rectf rect = child->getScreenRect(precise);
			bounds.include(rect);
		}
	}
	
	return bounds;
}

Rectf Node2d::getBounds() const
{
	float max = std::numeric_limits<float>::max();
	float min = std::numeric_limits<float>::min();
	vec2 max_pt = vec3(max);
	vec2 min_pt = vec3(min);
	Rectf boundary(max_pt, min_pt);
	
	//NodeDeque::const_reverse_iterator itr;
	for (auto itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr) {
		Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
		if (child) {
			Rectf rect = child->getBounds();
			boundary.include(rect);
		}
	}
	
	return boundary;
}

vec2 Node2d::viewportToObject( const vec2& pt, const Node2d& object )
{
	return vec2(0);
}
/*
vec2 Node2d::viewportToObject( const vec2& pt, const Node2d& object )
{
	return viewportToObject(pt, object.getWorldTransform().invertCopy(), gl::getViewport());
}
*/

vec2 Node2d::objectToViewport( const vec2& pt, const Node2d& object )
{
	return vec2(0);
}
/*
vec2 Node2d::objectToViewport( const vec2& pt, const Node2d& object )
{
	return objectToViewport(pt, object.getWorldTransform(), gl::getViewport());
}
*/

vec2 Node2d::viewportToObject( const vec2& pt, const mat3& inverse_transform, const Area& viewport )
{
//	vec2 p(pt);
	
//	p.y = (viewport.getHeight() - p.y);	// do this in a more reliably way (??)
	
	// -transform normalized coordinates [-1, 1]
//	vec2 x(pt);
//	vec2 p = inverse_transform * x;
//
//	return p;
	return vec2(0);
}

vec2 Node2d::objectToViewport( const vec2& pt, const mat3& transform, const Area& viewport )
{
	vec2 b = glm::xy( transform * vec3(pt, 0) );
	
	vec2 result;
	result.x = viewport.getX1() + viewport.getWidth() * (b.x + 1.0f) / 2.0f;
	result.y = viewport.getY1() + viewport.getHeight() * (1.0f - (b.y + 1.0f) / 2.0f);
	
	return result;
}

vec2 Node2d::parentToObject( const vec2& pt, const Node2d& object )
{
//	mat4 obj_trans_inv = glm::inverse(object.getTransform());
//	vec4 pt_trans = obj_trans_inv * vec4(pt, 1);
//	return glm::xyz(pt_trans);
	
	mat3 obj_trans_inv = glm::inverse(object.getTransform());
	vec3 pt_trans = obj_trans_inv * vec3(pt, 0);
	return glm::xy(pt_trans);
}

vec2 Node2d::objectToParent( const vec2& pt, const Node2d& object )
{
	return glm::xy( object.getTransform() * vec3(pt,0) );
}

vec2 Node2d::worldToObject( const vec2& pt, const Node2d& object )
{
	mat3 world_trans_inv = glm::inverse(object.getWorldTransform());
	vec3 pt_trans = world_trans_inv * vec3(pt, 0);
	return glm::xy(pt_trans);
}

vec2 Node2d::objectToWorld( const vec2& pt, const Node2d& object )
{
	return glm::xy( object.getWorldTransform() * vec3(pt,0) );
}

bool Node2d::sortHorizontally(const NodeRef& lhs, const NodeRef& rhs)
{
	Node2dRef left = std::dynamic_pointer_cast<Node2d>(lhs);
	Node2dRef right = std::dynamic_pointer_cast<Node2d>(rhs);
	return (left->mPosition.x < right->mPosition.x);
}

bool Node2d::sortVertically(const NodeRef& lhs, const NodeRef& rhs)
{
	Node2dRef left = std::dynamic_pointer_cast<Node2d>(lhs);
	Node2dRef right = std::dynamic_pointer_cast<Node2d>(rhs);
	return (left->mPosition.y < right->mPosition.y);
}

bool Node2d::sortBySize(const NodeRef& lhs, const NodeRef& rhs)
{
	Node2dRef left = std::dynamic_pointer_cast<Node2d>(lhs);
	Node2dRef right = std::dynamic_pointer_cast<Node2d>(rhs);
	return (left->mSize.length() < right->mSize.length());
}

#pragma mark auxiliary functions

namespace cg {
	
	//--------------------------------
	void alignHorizontally(Node2d& object, const float x, HorizontalAlignment_t type)
	{
		// re-align the children elements
		NodeDeque::const_iterator itr;
		for (itr = object.getChildren().begin(); itr != object.getChildren().end(); ++itr) {
			Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
			vec2 pos = child->getPosition();
			vec2 pivot = child->getPivotPercentage();
			vec2 size = child->getSize();
			switch (type) {
				case HorizontalAlignment::LEFT:
					child->setPosition(x + (pivot.x * size.x), pos.y);
					break;
				case HorizontalAlignment::CENTER:
					child->setPosition(x, pos.y);
					break;
				case HorizontalAlignment::RIGHT:
					child->setPosition(x - (1.0 - pivot.x) * size.x, pos.y);
					break;
			}
		}
	}
	
	void alignChildrenLeftHorizontally(Node2d& object, const float x) {
		alignHorizontally(object, x, HorizontalAlignment::LEFT);
	}
	
	void alignChildrenCenterHorizontally(Node2d& object, const float x) {
		alignHorizontally(object, x, HorizontalAlignment::CENTER);
	}
	
	void alignChildrenRightHorizontally(Node2d& object, const float x) {
		alignHorizontally(object, x, HorizontalAlignment::RIGHT);
	}
	
	//--------------------------------
	void alignVertically(Node2d& object, const float y, VerticalAlignment_t type)
	{
		NodeDeque::iterator itr;
		for (itr = object.getChildren().begin(); itr != object.getChildren().end(); ++itr) {
			Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
			vec2 pos = child->getPosition();
			vec2 pivot = child->getPivotPercentage();
			vec2 size = child->getSize();
			switch (type) {
				case VerticalAlignment::TOP:
					child->setPosition(pos.x, y + (pivot.y * size.y));
					break;
				case VerticalAlignment::MIDDLE:
					child->setPosition(pos.x, y);
					break;
				case VerticalAlignment::BOTTOM:
					child->setPosition(pos.x, y - (1.0 - pivot.y) * size.y);
					break;
			}
		}
	}
	
	void alignChildrenTopVertically(Node2d& object, const float y) {
		alignVertically(object, y, VerticalAlignment::TOP);
	}
	
	void alignChildrenCenterVertically(Node2d& object, const float y) {
		alignVertically(object, y, VerticalAlignment::MIDDLE);
	}
	
	void alignChildrenBottomVertically(Node2d& object, const float y) {
		alignVertically(object, y, VerticalAlignment::BOTTOM);
	}
	
	//--------------------------------
	void distributeHorizontally(Node2d& object, HorizontalAlignment_t type)
	{
		// compute bounding rectangle for all children
		Rectf full_boundary = object.getBounds();
		uint32_t child_count = object.getChildren().size();
		std::deque<NodeRef> children = object.getChildren();
		std::sort(children.begin(), children.end(), Node2d::sortHorizontally);
		
		// perform distribution...
		NodeDeque::iterator itr;
		if (type == HorizontalAlignment::LEFT) {
			float last_child_width = std::dynamic_pointer_cast<Node2d>(children.back())->getSize().x;
			float child_x = full_boundary.getX1();
			float increment = (full_boundary.getWidth() - last_child_width) / static_cast<float>(child_count-1);
			for (itr = children.begin(); itr != children.end(); ++itr) {
				Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
				vec2 pos = child->getPosition();
				vec2 pivot = child->getPivotPercentage();
				vec2 size = child->getSize();
				child->setPosition(child_x + (pivot.x * size.x), pos.y);
				child_x += increment;
			}
		}
		else if (type == HorizontalAlignment::CENTER) {
			float first_child_width = std::dynamic_pointer_cast<Node2d>(children.front())->getSize().x;
			float first_child_pivot_x = std::dynamic_pointer_cast<Node2d>(children.front())->getPivotPercentage().x;
			float last_child_width = std::dynamic_pointer_cast<Node2d>(children.back())->getSize().x;
			float last_child_pivot_x = std::dynamic_pointer_cast<Node2d>(children.back())->getPivotPercentage().x;
			float child_x = full_boundary.getX1() + (first_child_pivot_x * first_child_width);
			float full_width = full_boundary.getWidth() - (first_child_pivot_x * first_child_width) - (last_child_pivot_x * last_child_width);
			float increment = full_width / static_cast<float>(child_count-1);
			for (itr = children.begin(); itr != children.end(); ++itr) {
				Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
				vec2 pos = child->getPosition();
				child->setPosition(child_x, pos.y);
				child_x += increment;
			}
		}
		else if (type == HorizontalAlignment::RIGHT) {
			float first_child_width = std::dynamic_pointer_cast<Node2d>(children.front())->getSize().x;
			float child_x = full_boundary.getX1() + first_child_width;
			float increment = (full_boundary.getWidth() - first_child_width) / static_cast<float>(child_count-1);
			for (itr = children.begin(); itr != children.end(); ++itr) {
				Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
				vec2 pos = child->getPosition();
				vec2 pivot = child->getPivotPercentage();
				vec2 size = child->getSize();
				child->setPosition(child_x - (1.0 - pivot.x) * size.x, pos.y);
				child_x += increment;
			}
		}
	}
	
	void distributeChildrenLeftHorizontally(Node2d& object) {
		distributeHorizontally(object, HorizontalAlignment::LEFT);
	}
	
	void distributeChildrenCenterHorizontally(Node2d& object) {
		distributeHorizontally(object, HorizontalAlignment::CENTER);
	}
	
	void distributeChildrenRightHorizontally(Node2d& object) {
		distributeHorizontally(object, HorizontalAlignment::RIGHT);
	}
	
	//--------------------------------
	void distributeVertically(Node2d& object, VerticalAlignment_t type)
	{
		// compute bounding rectangle for all children
		Rectf full_boundary = object.getBounds();
		uint32_t child_count = object.getChildren().size();
		std::deque<NodeRef> children = object.getChildren();
		std::sort(children.begin(), children.end(), Node2d::sortVertically);
		
		// perform distribution...
		NodeDeque::iterator itr;
		if (type == VerticalAlignment::TOP) {
			float last_child_height = std::dynamic_pointer_cast<Node2d>(children.back())->getSize().y;
			float child_y = full_boundary.getY1();
			float increment = (full_boundary.getHeight() - last_child_height) / static_cast<float>(child_count-1);
			for (itr = children.begin(); itr != children.end(); ++itr) {
				Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
				vec2 pos = child->getPosition();
				vec2 pivot = child->getPivotPercentage();
				vec2 size = child->getSize();
				child->setPosition(pos.x, child_y + (pivot.y * size.y));
				child_y += increment;
			}
		}
		else if (type == VerticalAlignment::MIDDLE) {
			float first_child_height = std::dynamic_pointer_cast<Node2d>(children.front())->getSize().y;
			float first_child_pivot_y = std::dynamic_pointer_cast<Node2d>(children.front())->getPivotPercentage().y;
			float last_child_height = std::dynamic_pointer_cast<Node2d>(children.back())->getSize().y;
			float last_child_pivot_y = std::dynamic_pointer_cast<Node2d>(children.back())->getPivotPercentage().y;
			float child_y = full_boundary.getY1() + (first_child_pivot_y * first_child_height);
			float full_height = full_boundary.getHeight() - (first_child_pivot_y * first_child_height) - (last_child_pivot_y * last_child_height);
			float increment = full_height / static_cast<float>(child_count-1);
			for (itr = children.begin(); itr != children.end(); ++itr) {
				Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
				vec2 pos = child->getPosition();
				child->setPosition(pos.x, child_y);
				child_y += increment;
			}
		}
		else if (type == VerticalAlignment::BOTTOM) {
			float first_child_height = std::dynamic_pointer_cast<Node2d>(children.front())->getSize().y;
			float child_y = full_boundary.getY1() + first_child_height;
			float increment = (full_boundary.getHeight() - first_child_height) / static_cast<float>(child_count-1);
			for (itr = children.begin(); itr != children.end(); ++itr) {
				Node2dRef child = std::dynamic_pointer_cast<Node2d>(*itr);
				vec2 pos = child->getPosition();
				vec2 pivot = child->getPivotPercentage();
				vec2 size = child->getSize();
				child->setPosition(pos.x, child_y - (1.0 - pivot.y) * size.y);
				child_y += increment;
			}
		}
	}
	
	void distributeChildrenTopVertically(Node2d& object) {
		distributeVertically(object, VerticalAlignment::TOP);
	}
	
	void distributeChildrenMiddleVertically(Node2d& object) {
		distributeVertically(object, VerticalAlignment::MIDDLE);
	}
	
	void distributeChildrenBottomVertically(Node2d& object) {
		distributeVertically(object, VerticalAlignment::BOTTOM);
	}
}
