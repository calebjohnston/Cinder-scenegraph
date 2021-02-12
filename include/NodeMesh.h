#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Color.h"
#include "cinder/Camera.h"
#include "cinder/Rect.h"
#include "cinder/TriMesh.h"

#include "Node3d.h"

namespace scene {

typedef std::shared_ptr<class NodeMesh> NodeMeshRef;		//!< A shared pointer to a Node3d instance
typedef std::shared_ptr<const NodeMesh> NodeMeshConstRef;	//!< A shared pointer to a constant Node3d instance
typedef std::weak_ptr<NodeMesh> NodeMeshWeakRef;			//!< A weak pointer to a Node3d instance

/**
 * @brief Node3d type that includes a TriMesh object
 *
 * @see scene::Node3d
 * @see ci::TriMesh
 */
class NodeMesh : public scene::Node3d {
public:
	NodeMesh(const ci::TriMesh& mesh = ci::TriMesh(), const std::string& name = "NodeMesh", const bool active = true);
	virtual ~NodeMesh();
	
	/** @inherit */
	virtual void setup();
	
	/** @inherit */
	virtual void update( double elapsed );
	
	/** @inherit */
	virtual void draw();
	
	ci::CameraPersp mCamera;	// TEMPORARY
	
//	virtual void setScreenRect(const ci::Rectf& bounds, const float depth = 0.0);
	virtual ci::Rectf getScreenRect(const ci::Matrix44f& MVP, const ci::Area& viewport, bool precise = true) const;
	virtual ci::AxisAlignedBox3f getBounds() const;
	virtual ci::AxisAlignedBox3f getMeshBounds() const;
	
	inline void setMeshColor(const ci::ColorA& color) { mMeshColor = color; }
	inline ci::ColorA getMeshColor() const { return mMeshColor; }
	
	virtual bool mouseMove( ci::app::MouseEvent event );
	virtual bool mouseDown( ci::app::MouseEvent event );
	virtual bool mouseDrag( ci::app::MouseEvent event );
	virtual bool mouseUp( ci::app::MouseEvent event );
	
	// stream logging support
	friend std::ostream& operator<<(std::ostream& lhs, const NodeMesh& rhs) {
		return lhs << "[NodeMesh name=" << rhs.mName << ", position=" << rhs.mPosition << ", children=" << rhs.mChildren.size() << "]";
	}
	
protected:
	bool			mIsDragged;
	ci::Vec2f		mMouseOffset;
	ci::Rectf		mScreenRect;	//!< The rect object that describes the node shape in screen space
	ci::TriMesh		mMesh;			//!< The 3d triangle mesh object 
	ci::ColorA		mMeshColor;		//!< Color given to the mesh object
	ci::Vec2f		mMousePos;		//!< Offset within the 3D object bounds
};
	
}
