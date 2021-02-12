#include "cinder/gl/gl.h"
#include "cinder/Ray.h"

#include "NodeMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace scene;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	Produce test cases for deletions
//
///////////////////////////////////////////////////////////////////////////

NodeMesh::NodeMesh(const ci::TriMesh& mesh, const std::string& name, const bool active)
:	Node3d(name, active), mMesh(mesh), mMeshColor(ColorA::white()), mMousePos(Vec2f::zero()), mIsDragged(false)
{
}

NodeMesh::~NodeMesh()
{
}

void NodeMesh::setup()
{
	//mMesh
}

void NodeMesh::update(double elapsed)
{
}

void NodeMesh::draw()
{
	gl::SaveColorState colorState;
	gl::color(mMeshColor);
	gl::draw(mMesh);
}

/*
void	NodeMesh::setScreenRect(const Rectf& bounds, const float depth)
{
	// TODO: implement this method...
}
 */

Rectf NodeMesh::getScreenRect(const Matrix44f& MVP, const Area& viewport, bool precise) const
{
	Rectf rect = Node3d::getScreenRect(MVP, viewport, precise);
	
	Matrix44f composed_transform = MVP * mWorldTransform;
	
	if (precise) {
		std::vector<Vec3f> vertices = mMesh.getVertices();
		std::vector<Vec2f> screen_points;
		screen_points.resize(vertices.size());
		
		std::vector<Vec3f>::const_iterator input_iter;
		std::vector<Vec2f>::iterator output_iter = screen_points.begin();
		for (input_iter = vertices.begin(); input_iter != vertices.end(); input_iter++, output_iter++) {
			*output_iter = Node3d::objectToViewport( *input_iter, composed_transform, viewport );
		}
		
		rect.include(screen_points);
	}
	else {
		AxisAlignedBox3f aabb = Node3d::getBounds();
		AxisAlignedBox3f mesh_bounds = mMesh.calcBoundingBox();
		aabb.include(mesh_bounds);
		
		Vec2f screen_min = Node3d::objectToViewport( aabb.getMin(), composed_transform, viewport );
		Vec2f screen_max = Node3d::objectToViewport( aabb.getMax(), composed_transform, viewport );
		rect.include(screen_min);
		rect.include(screen_max);
	}
	
	return rect;
}

AxisAlignedBox3f NodeMesh::getBounds() const
{
	AxisAlignedBox3f aabb = Node3d::getBounds();
	AxisAlignedBox3f mesh_bounds = mMesh.calcBoundingBox().transformed(mTransform);
	
	aabb.include(mesh_bounds);
	
	return aabb;
}

AxisAlignedBox3f NodeMesh::getMeshBounds() const
{
	return mMesh.calcBoundingBox().transformed(mTransform);
}

bool NodeMesh::mouseMove(MouseEvent event)
{
	// The event specifies the mouse coordinates in screen space, and our
	// node position is specified in parent space. So, transform coordinates
	// from one space to the other using the built-in methods.
	
	// check if mouse is inside node (screen space -> object space)
	//	Vec2f o = Node2d::viewportToObject(event.getPos(), *this);
	Vec2f o = Vec2f(event.getPos());	
	Matrix44f transform = mCamera.getProjectionMatrix() * mCamera.getModelViewMatrix();
	Area viewport = gl::getViewport();
	if (getScreenRect(transform, viewport, true).contains(o)) {
		mMeshColor = ColorA(0, 1, 0, 1);
		return true;
	} else {
		mMeshColor = ColorA(1, 1, 1, 1);
		return false;
	}
}

bool NodeMesh::mouseDown(MouseEvent event)
{
	// The event specifies the mouse coordinates in screen space, and our
	// node position is specified in parent space. So, transform coordinates
	// from one space to the other using the built-in methods.
	
	// check if we clicked inside node (screen space -> object space)
	//	Vec2f o = Node2d::viewportToObject(event.getPos(), *this);
	Vec2f pos = Vec2f(event.getPos());
	Matrix44f transform = mCamera.getProjectionMatrix() * mCamera.getModelViewMatrix();
	Area viewport = gl::getViewport();
	Rectf rect = getScreenRect(transform, viewport, true);
	if (!rect.contains(pos)) return false;
	
//	mFillColor = mFillSelectedColor;
	
	// calculate click offset
	if (mParent.lock()) {
//		Vec3f local_position = Node3d::viewportToObject(pos, static_cast<const Node3d&>(*(mParent.lock().get())));
//		mMousePos = local_position.xy() - getPosition();
		mMousePos = rect.getUpperLeft() - pos;
	}
	else {
		mMousePos = rect.getCenter();
	}
	mIsDragged = true;
	
	return true;
}

bool NodeMesh::mouseDrag(MouseEvent event)
{
	if (!mIsDragged) return false;
	
//	if (mParent.lock()) {
//		Vec3f local_position = Node3d::viewportToObject(Vec2f(event.getPos()), static_cast<const Node3d&>(*(mParent.lock().get())));
//		Vec2f p = local_position.xy() - mMousePos;
//		setPosition(p);
		
		Vec2f pos(event.getPos());
		Area viewport = gl::getViewport();
	
//	float imagePlaneApectRatio = viewport.getWidth() / viewport.getHeight();
//	( float uPos, float vPos )
//	float s = ( uPos - 0.5f ) * imagePlaneApectRatio;
//	float t = ( vPos - 0.5f );
//	float viewDistance = imagePlaneApectRatio / math<float>::abs( mFrustumRight - mFrustumLeft ) * mNearClip;
//	return Ray( mEyePoint, ( mU * s + mV * t - ( mW * viewDistance ) ).normalized() );
	
		Ray r = mCamera.generateRay(pos.x/viewport.getWidth(), (viewport.getHeight()-pos.y)/viewport.getHeight(), mCamera.getAspectRatio());
		
		Vec3f n(mCamera.getEyePoint());
	n.normalize();
	Vec3f dVector = getPosition();
		float distance = -dVector.dot(n);
		Vec3f origin = r.getOrigin();
		Vec3f direction = r.getDirection();
		
		// calculate intersection point.
		//t = -(AX0 + BY0 + CZ0 + D) / (AXd + BYd + CZd)
		float t_hit = -(n.x*origin.x + n.y*origin.y + n.z*origin.z + distance) / (n.x*direction.x + n.y*direction.y + n.z*direction.z);
		Vec3f intersection = origin + (direction * t_hit);
		
		// apply the new translation values
//		intersection = mWorldTransform.inverted().transformPoint(intersection);
	//
		Matrix44f parent_trans = mWorldTransform * mTransform.inverted();
	parent_trans.invert();
	intersection = parent_trans.transformPoint(intersection);
		setPosition(intersection);
	//
	
//	setPosition(Node3d::worldToObject(intersection, *this));
	
//	}
//	else {
//		setPosition(mMousePos);
//	}
	
	return true;
}

bool NodeMesh::mouseUp(MouseEvent event)
{
	mIsDragged = false;
	
//	mFillColor = mFillUnselectedColor;
	
	return false;
}
