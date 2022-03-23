#include <algorithm>
#include <functional>
#include <limits>

#include "cinder/gl/gl.h"
#include "cinder/gl/wrapper.h"
#include "glm/gtx/vec_swizzle.hpp"

#include "Node3d.h"

using namespace ci;
using namespace std;
using namespace scene;

///////////////////////////////////////////////////////////////////////////
//
// TODO:
//
///////////////////////////////////////////////////////////////////////////

Node3dRef Node3d::create(const std::string& name, const bool active)
{
	return Node3dRef( new Node3d( name, active ) );
}

Node3d::Node3d(const std::string& name, const bool active)
:	NodeBase(name, active), mSize(0), mPosition(0),
	mScale(1), mPivot(0), mTransform(1),
	mWorldTransform(1), mRotation()
{
}

Node3d::~Node3d()
{
}

vec3 Node3d::getPivotPercentage()
{
	if (mSize.length() > 0.0f) {
		return mPivot / mSize;
	}
	else {
		return vec3(0);
	}
}

void Node3d::deepTransform(const mat4& world)
{
	// update transform matrix by calling derived class's function
	transform();
	
	// calculate world transform matrix
	mWorldTransform = world * mTransform;
	
	// do the same for all children
	//NodeDeque::iterator itr;
	for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		std::shared_ptr<Node3d> child = std::dynamic_pointer_cast<Node3d>(*itr);
		child->deepTransform(mWorldTransform);
	}
}

void Node3d::deepDraw() {}
/*
void Node3d::deepDraw()
{
	if (!mIsActive) return;
	
	// let derived class know we are about to draw stuff
	pre_draw();
	
	// update transform matrix by calling derived class's function
	// (do it again here in case this node has no parent and thus wasn't updated)
	transform();
	
	// apply model-view transform
	gl::pushModelView();
//	gl::multModelView( mTransform );
	gl::multModelView( mWorldTransform );	// TODO: convert to glm version
	
	// draw this node by calling derived class
	draw();
	gl::popModelView();
	
	// draw this node's children
	//typename NodeDeque::iterator itr;
	for (auto itr = mChildren.begin(); itr != mChildren.end(); ++itr) {
		(*itr)->deepDraw();
	}
	
	// restore transform
//	gl::popModelView();
	
	// let derived class know we are done drawing
	post_draw();
}
*/

void Node3d::transform()
{
	if (!mTransformIsDirty) return;
	
	mTransform = mat4(1);
	mTransform = glm::translate(mTransform, mPosition);
	mTransform *= glm::toMat4(mRotation);
	mTransform = glm::translate(mTransform, mScale);
	mTransform = glm::translate(mTransform, -mPivot);
	mTransformIsDirty = false;
}

void Node3d::setRotation( float angle_x, float angle_y, float angle_z, bool use_degrees )
{
	if (use_degrees) {
		angle_x *= 180.0/M_PI;
		angle_y *= 180.0/M_PI;
		angle_z *= 180.0/M_PI;
	}
	
	quat xrot = glm::angleAxis(angle_x, vec3(1,0,0));
	quat yrot = glm::angleAxis(angle_y, vec3(0,1,0));
	quat zrot = glm::angleAxis(angle_z, vec3(0,0,1));
	mRotation = xrot * yrot * zrot;
	mTransformIsDirty = true;
}

AxisAlignedBox Node3d::getBounds() const
{
	float max = std::numeric_limits<float>::max();
	float min = std::numeric_limits<float>::min();
	vec3 max_pt = vec3(max);
	vec3 min_pt = vec3(min);
	AxisAlignedBox aabb(max_pt, min_pt);
	
	NodeDeque::const_reverse_iterator itr;
	for (itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr) {
		Node3dRef child = std::dynamic_pointer_cast<Node3d>(*itr);
		if (child) {
			AxisAlignedBox bounds = child->getBounds();
			aabb.include(bounds);
		}
	}
	
	return aabb;
}

void Node3d::setScreenRect(const Rectf& bounds, const float depth)
{
	/** TODO: IMPLEMENT */
}

Rectf Node3d::getScreenRect(const mat4& MVP, const Area& viewport, bool precise) const
{
	float max = std::numeric_limits<float>::max();
	float min = std::numeric_limits<float>::min();
	vec3 max_pt = vec3(max);
	vec3 min_pt = vec3(min);
	Rectf rect(max_pt, min_pt);
	
	NodeDeque::const_reverse_iterator itr;
	for (itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr) {
		Node3dRef child = std::dynamic_pointer_cast<Node3d>(*itr);
		if (child) {
			Rectf bounds = child->getScreenRect(MVP, viewport, precise);
			rect.include(bounds);
		}
	}
	
	return rect;
}

vec3 Node3d::unproject(const vec3& pt, const Node3d& object)
{
	return vec3(0);
}
/*
vec3 Node3d::unproject(const vec3& pt, const Node3d& object)
{
	// since we don't have GLU, we have to do the calculation ourselves:
	// -find the inverse modelview-projection-matrix
	
	// TODO: convert to glm version AND factor out the GL state references
	mat4 projection = gl::getProjection();
	mat4 modelview = gl::getModelView();
	mat4 inverse_transform = projection * modelview * object.getWorldTransform();
	inverse_transform.invert();
	
	Area viewport = gl::getViewport();
	
	return Node3d::unproject(pt, inverse_transform, viewport);
}
*/

vec3 Node3d::unproject(const vec3& pt, const mat4& composed_inverse, const Area& viewport )
{
	// transform to normalized coordinates [-1, 1]
	vec4 x;
	x.x = (pt.x - viewport.getX1()) / viewport.getWidth()*2.0f - 1.0f;
	x.y = (pt.y - viewport.getY1()) / viewport.getHeight()*2.0f - 1.0f;
	x.z = 2.0f * pt.z - 1.0f;
	x.w = 1.0f;
	
	// transform to objects coordinates
	vec4 b = composed_inverse * x;
	if (b.w != 0.0f) b.w = 1.0f / b.w;
	return vec3(b.x*b.w, b.y*b.w, b.z*b.w);
}

// TODO: TEST
vec3 Node3d::viewportToObject( const vec2& pt, const Node3d& object)
{
	return vec3(0);
}
/*
vec3 Node3d::viewportToObject( const vec2& pt, const Node3d& object)
{
	mat4 projection = gl::getProjection();
	mat4 modelview = gl::getModelView();
	Area viewport = gl::getViewport();
	
	mat4 inverse_transform = projection * modelview * object.getWorldTransform();
	inverse_transform.invert();
	
	return viewportToObject(pt, inverse_transform, viewport);
}
 */

vec3 Node3d::viewportToObject( const vec2& pt, const mat4& composed_inverse, const Area& viewport )
{
	vec3 p(pt, 0);
	
	// adjust y (0,0 is lowerleft corner in OpenGL)
	p.y = (viewport.getHeight() - p.y);
	
	// near plane intersection
	p.z = 0.0f;
	vec3 p0 = Node3d::unproject(p, composed_inverse, viewport);
	
	// far plane intersection
	p.z = 1.0f;
	vec3 p1 = Node3d::unproject(p, composed_inverse, viewport);
	
	// find (x, y) coordinates
	float alpha = (p1.z == p0.z)? 0: (0.0f - p0.z) / (p1.z - p0.z);
	
	return lerp(p0, p1, alpha);
}

vec2 Node3d::objectToViewport( const vec3& pt, const Node3d& object )
{
	return vec2(0);
}
/*
vec2 Node3d::objectToViewport( const vec3& pt, const Node3d& object )
{
	mat4 projection = gl::getProjection();
	mat4 modelview = gl::getModelView();
	Area viewport = gl::getViewport();
	
	mat4 transform = projection * modelview * object.getWorldTransform();
	
	return objectToViewport(pt, transform, viewport);
}
*/

vec2 Node3d::objectToViewport( const vec3& pt, const mat4& composed, const Area& viewport )
{	
	// get into clip space ...
	vec4 x(pt, 1.0f);
	vec4 b = composed * x;
	
	// then into normalize device space ...
	if (b.w != 0.0f) b.w = 1.0f / b.w;
	b.x *= b.w;
	b.y *= b.w;
	b.z *= b.w;
	
	// now into window/viewport space ...
	vec2 result;
	result.x = viewport.getX1() + viewport.getWidth() * (b.x + 1.0f) / 2.0f;
	result.y = viewport.getY1() + viewport.getHeight() * (1.0f - (b.y + 1.0f) / 2.0f);
	
	return result;
}

// TODO: TEST
vec3 Node3d::parentToObject( const vec3& pt, const Node3d& object )
{
	mat4 obj_trans_inv = glm::inverse(object.getTransform());
	vec4 pt_trans = obj_trans_inv * vec4(pt, 1);
	return glm::xyz(pt_trans);
}

// TODO: TEST
vec3 Node3d::objectToParent( const vec3& pt, const Node3d& object )
{
	return glm::xyz( object.getTransform() * vec4(pt, 1) );
}

// TODO: TEST
vec3 Node3d::worldToObject( const vec3& pt, const Node3d& object )
{
	mat4 world_trans_inv = glm::inverse(object.getWorldTransform());
	return glm::xyz( world_trans_inv * vec4(pt, 1) );
}

// TODO: TEST
vec3 Node3d::objectToWorld( const vec3& pt, const Node3d& object )
{
	return glm::xyz( object.getWorldTransform() * vec4(pt, 1) );
}

bool Node3d::sortPositionX(const NodeRef& lhs, const NodeRef& rhs)
{
	Node3dRef left = std::dynamic_pointer_cast<Node3d>(lhs);
	Node3dRef right = std::dynamic_pointer_cast<Node3d>(rhs);
	return (left->mPosition.x < right->mPosition.x);
}

bool Node3d::sortPositionY(const NodeRef& lhs, const NodeRef& rhs)
{
	Node3dRef left = std::dynamic_pointer_cast<Node3d>(lhs);
	Node3dRef right = std::dynamic_pointer_cast<Node3d>(rhs);
	return (left->mPosition.y < right->mPosition.y);
}

bool Node3d::sortPositionZ(const NodeRef& lhs, const NodeRef& rhs)
{
	Node3dRef left = std::dynamic_pointer_cast<Node3d>(lhs);
	Node3dRef right = std::dynamic_pointer_cast<Node3d>(rhs);
	return (left->mPosition.z < right->mPosition.z);
}

bool Node3d::sortBySize(const NodeRef& lhs, const NodeRef& rhs)
{
	Node3dRef left = std::dynamic_pointer_cast<Node3d>(lhs);
	Node3dRef right = std::dynamic_pointer_cast<Node3d>(rhs);
	return (left->mSize.length() < right->mSize.length());
}
