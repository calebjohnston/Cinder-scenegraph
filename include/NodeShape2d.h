#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/Shape2d.h"
#include "cinder/TriMesh.h"

#include "Node2d.h"

namespace scene {
	
typedef std::shared_ptr<class NodeShape2d> NodeShape2dRef;		//!< A shared pointer to a NodeShape2d instance
typedef std::shared_ptr<const NodeShape2d> NodeShape2dConstRef;	//!< A shared pointer to a constant NodeShape2d instance
typedef std::weak_ptr<NodeShape2d> NodeShape2dWeakRef;			//!< A weak pointer to a NodeShape2d instance

/**
 * @brief Node2d type that includes a 2d shape object in which to draw
 *
 * @see scene::Node2d
 * @see ci::Shape2d
 */
class NodeShape2d : public scene::Node2d {
public:
	NodeShape2d(const ci::Shape2d& shape = ci::Shape2d(), const std::string& name = "NodeShape2d", const bool active = true);
	virtual ~NodeShape2d();
	
	virtual void setup();
	/** Cinder update method */
	virtual void update( double elapsed );
	/** Cinder draw method */
	virtual void draw();
	
//	virtual void		setScreenRect(const ci::Rectf& bounds, const float depth = 0.0);
	virtual ci::Rectf	getScreenRect(bool precise = true) const;
	virtual ci::Rectf	getBounds() const;
	virtual ci::Rectf	getShapeBounds() const;
	
	virtual ci::Vec2f getAnchorPercentage();
	
	virtual ci::ColorA getFillColor(const ci::ColorA& color) { return mFillColor; }
	virtual void setFillColor(const ci::ColorA& color) { mFillColor = color; }
	
	virtual ci::ColorA getStrokeColor() { return mStrokeColor; }
	virtual void setStrokeColor(const ci::ColorA& color) { mStrokeColor = color; }
	
	virtual bool mouseMove( ci::app::MouseEvent event );
	virtual bool mouseDown( ci::app::MouseEvent event );
	virtual bool mouseDrag( ci::app::MouseEvent event );
	virtual bool mouseUp( ci::app::MouseEvent event );
	
	// stream logging support
	friend std::ostream& operator<<(std::ostream& lhs, const NodeShape2d& rhs) {
		return lhs << "[NodeShape2d name=" << rhs.mName << ", position=" << rhs.mPosition << ", children=" << rhs.mChildren.size() << "]";
	}
		
protected:
	bool			mIsDragged;				//!< Flag set when being dragged
	ci::TriMesh2d	mShapeMesh;				//!< Cached local copy of the trimesh for the shape
	ci::Shape2d		mShape;					//!< The shape object that describes the node appearance
	ci::Vec2f		mMouseOffset;			//!< Offset within the rectangle
	ci::ColorA		mFillColor;				//!< Color given to the object's fill
	ci::ColorA		mFillSelectedColor;		//!< Color given to the object's fill
	ci::ColorA		mFillUnselectedColor;	//!< Color given to the object's fill
	ci::ColorA		mStrokeColor;			//!< Color given to the object's stroke
};

}
