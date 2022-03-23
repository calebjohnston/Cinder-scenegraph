#pragma once

#include <string>
#include <iostream>
#include <deque>

#include "cinder/app/App.h"
#include "cinder/CinderMath.h"
#include "cinder/Matrix.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "NodeBase.h"

namespace scene {

class Node2d;
typedef std::shared_ptr<Node2d> Node2dRef;				//!< A shared pointer to a Node2d instance
typedef std::shared_ptr<const Node2d> Node2dConstRef;	//!< A shared pointer to a constant Node2d instance
typedef std::weak_ptr<Node2d> Node2dWeakRef;			//!< A weak pointer to a Node2d instance
	
/**
 * @brief Node2d is the base class for any scene graph which only contains 2D data
 * 
 * The Node2d defines all coordinates in 2d and computes all transformations using an affine matrix
 *
 * @see scene::NodeShape2d
 */
class Node2d : public NodeBase {
public:
	/** creates Node2d instance wrapped by STL shared pointer */
	static Node2dRef create(const std::string& name = "", const bool active = true);
	
	/** @inherit */
	virtual ~Node2d();
	
	//! returns the transformation matrix of this node
	const ci::mat3& getTransform() const { return mTransform; }
	
	//! returns the world transformation matrix of this node
	const ci::mat3& getWorldTransform() const { return mWorldTransform; }
	
	//! returns the 2d position of the node as a mutable reference
	ci::vec2&	position() { return mPosition; }
	//! returns the 2d position of the node
	ci::vec2	getPosition() const { return mPosition; }
	//! assigns the 2d position of the node using a 2D vector
	void		setPosition(const ci::vec2& pt) { mPosition = pt; mTransformIsDirty = true; }
	//! assigns the 2d position of the node using two floats
	void		setPosition(const float x, const float y) { mPosition = ci::vec2(x,y); mTransformIsDirty = true; }
	
	//! returns the 2d scale of the node as a mutable reference
	ci::vec2&	scale() { return mScale; }
	//! returns the 2d scale of the node
	ci::vec2	getScale() const { return mScale; }
	//! assigns the 2d uniform scale of the node using a single float
	void		setScale(const float scale) { mScale = ci::vec2(scale,scale); mTransformIsDirty = true; }
	//! assigns the 2d non-uniform scale of the node using a 2D vector
	void		setScale(const ci::vec2& scale) { mScale = scale; mTransformIsDirty = true; }
	//! assigns the 2d uniform scale of the node using two floats
	void		setScale(const float w, const float h) { mScale = ci::vec2(w,h); mTransformIsDirty = true; }
	
	//! returns the 2d rotation of the node as a mutable reference, expressed in radians
	float&		rotation() { return mRotation; }
	//! returns the 2d rotation of the node, expressed in radians
	float		getRotation() const { return mRotation; }
	//! assigns the 2d rotation of the node, expressed in radians by default
	void		setRotation(const float radians, const bool use_degrees = false);
	
	//! returns the 2d pivot point (or centroid) for the node as a mutable reference
	ci::vec2&	getPivot() { return mPivot; }
	//! returns the 2d pivot point (or centroid) for the node
	ci::vec2	getPivot() const { return mPivot; }
	//! assigns the 2d pivot point (or centroid) for the node using a 2D vector
	void		setPivot(const ci::vec2& pt) { mPivot = pt; mTransformIsDirty = true; }
	//! assigns the 2d pivot point (or centroid) for the node using two floats
	void		setPivot(const float x, const float y) { mPivot = ci::vec2(x,y); mTransformIsDirty = true; }
	
	//! assigns the 2d size of the node
	virtual void		setSize(const ci::vec2& size) { mSize = size; };
	//! returns the 2d size of the node (and it's contents --??)
	virtual ci::vec2	getSize() const { return mSize; };
	//! returns the rectangular boundary of the node (and it's contents --??)
	virtual ci::Rectf	getBounds() const;
	
	//! returns the 2d pivot (or centroid) of the node as percentage values computed based upon the node content size
	virtual ci::vec2	getPivotPercentage() const;
	//! assigns the 2d pivot point (or centroid) of the node as percentage values of the node's total size
	virtual void		setPivotPercentage(const ci::vec2& pt) { mPivot = pt * mSize; mTransformIsDirty = true; }
	
	
	//! assigns the contents of an object to a bounded region defined in screen space
	virtual void		setScreenRect(const ci::Rectf& bounds);
	
	//! returns the screen space boundary of the contens of the node
	virtual ci::Rectf	getScreenRect(bool precise = true) const;
	
	//! @inherit
	virtual void deepDraw();
	
	//! Performs a recursive tree traversal that computes the world transformation with respect to each node
	virtual void deepTransform(const ci::mat3& world = ci::mat3(1));
	
	//! Stream operator provides support for convenient logging
	friend std::ostream& operator<<(std::ostream& lhs, const Node2d& o) {
		return lhs << "[Node2d name=" << o.getName() << ", position=" << o.mPosition << ", children=" << o.mChildren.size() << "]";
	}
	
	/**
	 * Transforms the input 2d point from the viewport coordinate space to the local object coordinate
	 * space. This method performs a matrix-matrix multiplication and matrix inverse which are both
	 * expensive. If this method is needed to transform several points, use the other viewportToObject
	 * method instead.
	 *
	 * @param pt the 2d point to undergo a coordinate transformtion
	 * @param object the local coordinate system that the transformation will be relative to
	 * @return the 2d point in a coordinate space local to the input object
	 */
	static ci::vec2 viewportToObject( const ci::vec2& pt, const Node2d& object );
	
	/**
	 * Transforms the input 2d point from the local object coordinate space to the viewport coordinate
	 * space. This method performs a matrix-matrix multiplication and matrix inverse which are both
	 * expensive. If this method is needed to transform several points, use the other viewportToObject
	 * method instead.
	 *
	 * @param pt the 2d point to undergo a coordinate transformtion
	 * @param object the local coordinate system that the transformation will be relative to
	 * @return the 2d point in a coordinate space of the viewport
	 */
	static ci::vec2 objectToViewport( const ci::vec2& pt, const Node2d& object );
	
	/**
	 * Transforms the input 2d point from the viewport coordinate space to the local object coordinate
	 * space using the input transform and viewport area.
	 *
	 * @param pt the 2d point to undergo a coordinate transformtion
	 * @param composed the inverted modelview-projection-world transformation
	 * @param viewport the current viewport settings described as an Area object
	 * @return the 2d point in a coordinate space local to the input object
	 */
	static ci::vec2 viewportToObject( const ci::vec2& pt, const ci::mat3& inverse_transform, const ci::Area& viewport );
	
	/**
	 * Transforms the input 2d point from the local object coordinate space to the viewport coordinate
	 * space using the input transform and viewport area.
	 *
	 * @param pt the 2d point to undergo a coordinate transformtion
	 * @param composed the composed modelview-projection-world transformation
	 * @param viewport the current viewport settings described as an Area object
	 * @return the 2d point in a coordinate space of the viewport
	 */
	static ci::vec2 objectToViewport( const ci::vec2& pt, const ci::mat3& transform, const ci::Area& viewport );
		
	//! converts a 2d point between the parent's coordinate space to the node object coordinate space
	static ci::vec2 parentToObject( const ci::vec2& pt, const Node2d& object );
	//! converts a 2d point between the local node object coordinate space to the parent object's coordinate space
	static ci::vec2 objectToParent( const ci::vec2& pt, const Node2d& object );
	//! converts a 2d point between the global world coordinate space to the node object's local coordinate space
	static ci::vec2 worldToObject( const ci::vec2& pt, const Node2d& object );
	//! converts a 2d point between the node object's local coordinate space and the global world coordinate space
	static ci::vec2 objectToWorld( const ci::vec2& pt, const Node2d& object );
		
	//! function for sorting nodes in a list based upon their relative x-axis positions
	static bool sortHorizontally(const NodeRef& lhs, const NodeRef& rhs);
	//! function for sorting nodes in a list based upon their relative y-axis positions
	static bool sortVertically(const NodeRef& lhs, const NodeRef& rhs);
	//! function for sorting nodes in a list based upon their relative sizes
	static bool sortBySize(const NodeRef& lhs, const NodeRef& rhs);
		
protected:
	/** @inherit */
	Node2d(const std::string& name = "", const bool active = true);

	bool				mTransformIsDirty;	//!< flag used to manage transformation matrix cache
	ci::vec2			mPosition;			//!< the 2D or 3D position of the node
	ci::vec2			mScale;				//!< the 2D or 3D scale applied to the node
	ci::vec2			mPivot;				//!< the 2D or 3D location about which the node rotates
	ci::vec2			mSize;				//!< the 2D or 3D size of the node (based upon contents)
	float				mRotation;			//!< the floating point rotation represented in radians
	ci::mat3			mTransform;			//!< represents local transformation
	ci::mat3			mWorldTransform;	//!< represents world transformation
	
	//! @inherit
	virtual void transform();
};
		
#pragma mark auxiliary functions

//! Type that describes horizontal alignment
typedef enum HorizontalAlignment_t {
	LEFT = 0, CENTER = 1, RIGHT = 2
} HorizontalAlignment;

//! Type that describes vertical alignment
typedef enum VerticalAlignment_t {
	TOP = 0, MIDDLE = 1, BOTTOM = 2
} VerticalAlignment;

/**
 * Aligns all of the children of the object parameter with the vertical guide 
 * defined by the x parameter. The way in which the children are aligned is
 * described by the last parameter.
 * 
 * @param object the parent of all the nodes that will be aligned
 * @param x the vertical line to which the nodes will be aligned (in the coordinate space of the object)
 * @param type the way in which the nodes are aligned. 
 *		LEFT = align all objects based upon their left boundary
 *		CENTER = align all objects based upon their horizontal mid-point
 *		RIGHT = align all objects based upon their right boundary
 *
 * @see Node2d::alignVertically
 * @see Node2d::distributeHorizontally
 */
void alignHorizontally(Node2d& object, const float x, HorizontalAlignment_t type = CENTER);

/**
 * Horizontal align left for all child nodes of the input object
 * 
 * @see Node2d::alignHorizontally
 */
void alignChildrenLeftHorizontally(Node2d& object, const float x);
		
/**
 * Horizontal align center for all child nodes of the input object
 *
 * @see Node2d::alignHorizontally
 */
void alignChildrenCenterHorizontally(Node2d& object, const float x);

/**
 * Horizontal align right for all child nodes of the input object
 *
 * @see Node2d::alignHorizontally
 */
void alignChildrenRightHorizontally(Node2d& object, const float x);

/**
 * Aligns all of the children of the object parameter with the horizontal guide
 * defined by the y parameter. The way in which the children are aligned is
 * described by the last parameter.
 *
 * @param object the parent of all the nodes that will be aligned
 * @param y the horizontal line to which the nodes will be aligned (in the coordinate space of the object)
 * @param type the way in which the nodes are aligned.
 *		TOP = align all objects based upon their top boundary
 *		MIDDLE = align all objects based upon their vertical mid-point
 *		BOTTOM = align all objects based upon their bottom boundary
 *
 * @see Node2d::alignHorizontally
 * @see Node2d::distributeVertically
 */
void alignVertically(Node2d& object, const float y, VerticalAlignment_t type = MIDDLE);

/**
 * Vertical align top for all child nodes of the input object
 *
 * @see Node2d::alignVertically
 */
void alignChildrenTopVertically(Node2d& object, const float y);
		
/**
 * Vertical align middle for all child nodes of the input object
 *
 * @see Node2d::alignVertically
 */
void alignChildrenCenterVertically(Node2d& object, const float y);
		
/**
 * Vertical align bottom for all child nodes of the input object
 *
 * @see Node2d::alignVertically
 */
void alignChildrenBottomVertically(Node2d& object, const float y);

/**
 * Evenly distributes all of the children of the object parameter horizontally.
 * The width over which the objects are distributed is equivilant to the total
 * distance between the first and the last child of the input node object.
 * The way in which the children are distributed is described by the last parameter.
 *
 * @param object the parent of all the nodes that will be distributed
 * @param type the way in which the nodes are distributed.
 *		LEFT = distributes all objects based upon their left boundary
 *		CENTER = distributes all objects based upon their horizontal mid-point
 *		RIGHT = distributes all objects based upon their right boundary
 *
 * @see Node2d::alignHorizontally
 * @see Node2d::distributeVertically
 */
void distributeHorizontally(Node2d& object, HorizontalAlignment_t type = CENTER);

/**
 * Horizontal distribute left for all child nodes of the input object
 *
 * @see Node2d::distributeHorizontally
 */
void distributeChildrenLeftHorizontally(Node2d& object);

/**
 * Horizontal distribute center for all child nodes of the input object
 *
 * @see Node2d::distributeHorizontally
 */
void distributeChildrenCenterHorizontally(Node2d& object);

/**
 * Horizontal distribute right for all child nodes of the input object
 *
 * @see Node2d::distributeHorizontally
 */
void distributeChildrenRightHorizontally(Node2d& object);

/**
 * Evenly distributes all of the children of the object parameter vertically.
 * The height over which the objects are distributed is equivilant to the total
 * distance between the first and the last child of the input node object.
 * The way in which the children are distributed is described by the last parameter.
 *
 * @param object the parent of all the nodes that will be distributed
 * @param type the way in which the nodes are distributed.
 *		TOP = distributes all objects based upon their top boundary
 *		MIDDLE = distributes all objects based upon their vertical mid-point
 *		BOTTOM = distributes all objects based upon their bottom boundary
 *
 * @see Node2d::alignVertically
 * @see Node2d::distributeHorizontally
 */
void distributeVertically(Node2d& object, VerticalAlignment_t type = MIDDLE);
		
/**
 * Vertical distribute top for all child nodes of the input object
 *
 * @see Node2d::distributeVertically
 */
void distributeChildrenTopVertically(Node2d& object);

/**
 * Vertical distribute middle for all child nodes of the input object
 *
 * @see Node2d::distributeVertically
 */
void distributeChildrenMiddleVertically(Node2d& object);

/**
 * Vertical distribute bottom for all child nodes of the input object
 *
 * @see Node2d::distributeVertically
 */
void distributeChildrenBottomVertically(Node2d& object);

}
