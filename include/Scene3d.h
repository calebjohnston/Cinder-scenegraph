#pragma once

#include <string>
#include <iostream>
#include <deque>

#include "cinder/app/App.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/CinderMath.h"
#include "cinder/Matrix.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "NodeBase.h"

namespace scene {

class Node3d;

typedef std::shared_ptr<Node3d> Node3dRef;		//!< A shared pointer to a Node3d instance
typedef std::shared_ptr<const Node3d> Node3dConstRef;	//!< A shared pointer to a constant Node3d instance
typedef std::weak_ptr<Node3d> Node3dWeakRef;			//!< A weak pointer to a Node3d instance
	
/**
 * @brief Node3d is the base class for any scene graph which only contains 3D data
 *
 * @see scene::Node
 * @see scene::NodeMesh
 */
class Node3d : public scene::NodeBase {
public:
	/** creates Node2d instance wrapped by STL shared pointer */
	static Node3dRef create(const std::string& name = "", const bool active = true);
	
	/** @inherit */
	virtual ~Node3d();
	
	//! returns the transformation matrix of this node
	const ci::mat4& getTransform() const { return mTransform; }
	
	//! returns the world transformation matrix of this node
	const ci::mat4& getWorldTransform() const { return mWorldTransform; }
	
	//! returns the 3d position of the node as a mutable reference
	ci::vec3&	position() { return mPosition; }
	//! returns the 3d position of the node
	ci::vec3	getPosition() const { return mPosition; }
	//! assigns the 3d position of the node using a 3D vector
	void		setPosition( const ci::vec3& pt ) { mPosition = pt; mTransformIsDirty = true; }
	//! assigns the 3d position of the node using three floats
	void		setPosition( const float x, const float y, const float z ) { mPosition = ci::vec3(x,y,z); mTransformIsDirty = true; }
	
	//! returns the 3d scale of the node as a mutable reference
	ci::vec3&	scale() { return mScale; }
	//! returns the 3d scale of the node
	ci::vec3	getScale() const { return mScale; }
	//! assigns the 3d uniform scale of the node using a single float
	void		setScale( const float scale ) { mScale = ci::vec3(scale, scale, scale); mTransformIsDirty = true; }
	//! assigns the 3d scale of the node using a 3D vector
	void		setScale( const ci::vec3& scale ) { mScale = scale; mTransformIsDirty = true; }
	//! assigns the 3d scale of the node using three floats
	void		setScale( const float x, const float y, const float z ) { mScale = ci::vec3(x,y,z); mTransformIsDirty = true; }
	
	//! returns the rotation of the node represented as a mutable reference to a quaternion
	ci::quat&	rotation() { return mRotation; }
	//! returns the rotation of the node represented as a quaternion
	ci::quat	getRotation() const { return mRotation; }
	//! assigns the 3d rotation of the node using an axis angle representation
	void		setRotation( float radians, const ci::vec3& axis = ci::vec3(0,0,1) ) { mRotation = glm::angleAxis(radians, axis); mTransformIsDirty = true; }
	//! assigns the 3d rotation of the node as a quaternion
	void		setRotation( const ci::quat& rot ) { mRotation = rot; mTransformIsDirty = true; }
	//! assigns the 3d rotation of the node using Euler angles
	void		setRotation( float angle_x, float angle_y, float angle_z, bool use_degrees = false );
	
	//! returns the 3d pivot point of the node as a mutable reference
	ci::vec3&	pivot() { return mPivot; }
	//! returns the 3d pivot point of the node
	ci::vec3	getPivot() const { return mPivot; }
	//! assigns the 3d pivot point of the node using a 3D vector
	void		setPivot( const ci::vec3& pt ) { mPivot = pt; mTransformIsDirty = true; }
	//! assigns the 3d pivot point of the node using three floats
	void		setPivot( const float x, const float y, const float z ) { mPivot = ci::vec3(x,y,z); mTransformIsDirty = true; }
	
	//! returns the 3d anchor (centroid) of the node as percentage values computed based upon the node content size
	virtual	ci::vec3	getPivotPercentage();
	//! assigns a 3d anchor point (or centroid) of the node, expressed as a percentage of the node content size
	virtual	void		setPivotPercentage(const ci::vec3& pt) { mPivot = pt * mSize; mTransformIsDirty = true; }
	
	//! returns the axis-aligned bounding box for the contents of the node
	virtual ci::AxisAlignedBox getBounds() const;
	
	
	//! assigns the contents of an object to a bounded region defined in screen space
	virtual void		setScreenRect(const ci::Rectf& bounds, const float depth = 0.0);
	
	/**
	 * Computes the screen space coverage of the Node3d and it's children using the input model-view projection
	 * matrix, the input viewport, and the precise boolean flag. If precise is set to false then an approximation,
	 * in model space, will be used for the computation -leading to faster calculation with less accurate results.
	 * Otherwise the calculation will be performed using all the geometry in the node and it's children nodes which
	 * is very accurate but far more expensive to compute.
	 * 
	 * @param MVP the composed model-view projection matrix to use in the projection transformation
	 * @param viewport the screen viewport coordinates that the rectangle will be computed with respect to
	 * @param precise flag used to determine whether the geometry vertices will be used or the extents of the bounding region
	 * @return the rectangle that bounds the node3d object (and it's children) in screen space
	 *
	 * @see Node3d::objectToViewport
	 */
	virtual ci::Rectf	getScreenRect(const ci::mat4& MVP, const ci::Area& viewport, bool precise = true) const;
	
	/**
	 * Produces a composed transformation matrix that describes the coordinate conversion from global to local coordinates.
	 * 
	 * @param world the world transformation of the parent node
 	 */
	virtual void deepTransform(const ci::mat4& world = ci::mat4(1));
	
	/** @inherit */
	virtual void deepDraw();
	
	//! Stream operator provides support for convenient logging
	friend std::ostream& operator<<(std::ostream& lhs, const Node3d& rhs) {
		return lhs << "[Node3d name=" << rhs.mName << ", position=" << rhs.mPosition << ", children=" << rhs.mChildren.size() << "]";
	}
		
	/**
	 * Transforms the input 3d point from the canonical view volume of eye coordinates to the local 
	 * object coordinate space. It uses the current OpenGL ModelView matrix, Projection matrix, and 
	 * current Viewport settings to perform the conversion. Too avoid these defaults, one could 
	 * optionally provide a composed matrix transform and viewport area instead.
	 *
	 * @param pt the 3d point to undergo a coordinate transformtion
	 * @param object the local coordinate system that the transformation will be relative to
	 * @return the 3d point in object space
	 * 
	 * @see Node3d::unproject
	 */
	static ci::vec3 unproject(const ci::vec3& pt, const Node3d& object);

	/**
	 * Transforms the input 2d point from the viewport coordinate space to the local object coordinate
	 * space (in 3d). It uses the current OpenGL ModelView matrix, Projection matrix, and current
	 * Viewport settings to perform the conversion. Too avoid these defaults, one could optionally provide
	 * a composed matrix transform and viewport area instead.
	 * 
	 * @param pt the 2d point to undergo a coordinate transformtion
	 * @param object the local coordinate system that the transformation will be relative to
	 * @return the 3d point in a coordinate space local to the input object
	 */
	static ci::vec3 viewportToObject( const ci::vec2& pt, const Node3d& object);

	/**
	 * Transforms the input 3d point from the local object coordinate space to the viewport coordinate
	 * space. It uses the current OpenGL ModelView matrix, Projection matrix, and current Viewport 
	 * settings to perform the conversion. Too avoid these defaults, one could optionally provide
	 * a composed matrix transform and viewport area instead.
	 *
	 * @param pt the 3d point to undergo a coordinate transformtion
	 * @param object the local coordinate system that the transformation will be relative to
	 * @return the 2d point in a coordinate space of the viewport
	 */
	static ci::vec2 objectToViewport( const ci::vec3& pt, const Node3d& object);
	
	/**
	 * Transforms the input 3d point from the canonical view volume of eye coordinates to the local
	 * object coordinate space using the composed transformation matrix and the input viewport.
	 *
	 * @param pt the 3d point to undergo a coordinate transformtion
	 * @param composed the inverted modelview-projection-world transformation
	 * @param viewport the current viewport settings described as an Area object
	 * @return the 3d point in object space
	 *
	 * @see Node3d::unproject
	 */
	static ci::vec3 unproject(const ci::vec3& pt, const ci::mat4& composed_inverse, const ci::Area& viewport);

	/**
	 * Transforms the input 2d point from the viewport coordinate space to the local object coordinate
	 * space (in 3d) using the composed transformation matrix and the input viewport. This function works
	 * by invoking the unproject method using the specified input parameters.
	 *
	 * @param pt the 2d point to undergo a coordinate transformtion
	 * @param composed the inverted modelview-projection-world transformation
	 * @param viewport the current viewport settings described as an Area object
	 * @return the 3d point in a coordinate space local to the input object
	 *
	 * @see Node3d::unproject
	 */
	static ci::vec3 viewportToObject( const ci::vec2& pt, const ci::mat4& composed_inverse, const ci::Area& viewport);
	
	/**
	 * Transforms the input 3d point from the local object coordinate space to the viewport coordinate
	 * space using the composed transformation matrix and the input viewport.
	 *
	 * @param pt the 3d point to undergo a coordinate transformtion
	 * @param composed the composed modelview-projection-world transformation
	 * @param viewport the current viewport settings described as an Area object
	 * @return the 2d point in a coordinate space of the viewport
	 */
	static ci::vec2 objectToViewport( const ci::vec3& pt, const ci::mat4& composed, const ci::Area& viewport);
		
	//! converts a 3d point between the parent's coordinate space to the node object coordinate space
	static ci::vec3 parentToObject( const ci::vec3& pt, const Node3d& object );
	//! converts a 3d point between the local node object coordinate space to the parent object's coordinate space
	static ci::vec3 objectToParent( const ci::vec3& pt, const Node3d& object );
	//! converts a 3d point between the global world coordinate space to the node object's local coordinate space
	static ci::vec3 worldToObject( const ci::vec3& pt, const Node3d& object );
	//! converts a 3d point between the node object's local coordinate space and the global world coordinate space
	static ci::vec3 objectToWorld( const ci::vec3& pt, const Node3d& object );
		
	//! function for sorting nodes in a list based upon their relative x-axis positions
	static bool sortPositionX(const NodeRef& lhs, const NodeRef& rhs);
	//! function for sorting nodes in a list based upon their relative y-axis positions
	static bool sortPositionY(const NodeRef& lhs, const NodeRef& rhs);
	//! function for sorting nodes in a list based upon their relative z-axis positions
	static bool sortPositionZ(const NodeRef& lhs, const NodeRef& rhs);
	//! function for sorting nodes in a list based upon their relative sizes
	static bool sortBySize(const NodeRef& lhs, const NodeRef& rhs);
	
protected:
	/** @inherit */
	Node3d(const std::string& name = "", const bool active = true);
	
	bool				mTransformIsDirty;	//!< flag used to manage transformation matrix cache
	ci::vec3			mPosition;			//!< the 2D or 3D position of the node
	ci::vec3			mScale;				//!< the 2D or 3D scale applied to the node
	ci::vec3			mPivot;			//!< the 2D or 3D location about which the node rotates
	ci::vec3			mSize;				//!< the 2D or 3D size of the node (based upon contents)
	ci::quat			mRotation;			//!< the quaternion rotation applied to the node
	ci::mat4			mTransform;			//!< represents local transformation
	ci::mat4			mWorldTransform;	//!< represents world transformation
	
	//! calculates the local transformation matrix using the current translation, scale, and rotation values.
	virtual void transform();
	
};
		
}
