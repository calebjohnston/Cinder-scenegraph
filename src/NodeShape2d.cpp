#include "cinder/gl/gl.h"
#include "cinder/Triangulate.h"

#include "NodeShape2d.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace scene;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	Produce test cases for deletions
//
///////////////////////////////////////////////////////////////////////////

NodeShape2d::NodeShape2d(const ci::Shape2d& shape, const std::string& name, const bool active)
:	Node2d(name, active), mShape(shape), mIsDragged(false), mStrokeColor(ColorA(1,0,0,1)),
	mFillSelectedColor(ColorA(0.9f,0.9f,0.9f,1.0f)), mFillUnselectedColor(ColorA::white()), mFillColor(ColorA::white())
{
}

NodeShape2d::~NodeShape2d()
{
}

void NodeShape2d::setup()
{
	mShapeMesh = Triangulator(mShape).calcMesh();
	mSize = mScale * mShape.calcPreciseBoundingBox().getSize();
//	mFillColor = mFillUnselectedColor;
}

void NodeShape2d::update(double elapsed)
{
	mSize = mScale * mShape.calcPreciseBoundingBox().getSize();
}

void NodeShape2d::draw()
{
	gl::SaveColorState colorState;
	gl::color(mFillColor);
	gl::drawSolid(mShape);
	gl::color(mStrokeColor);
	gl::draw(mShape);
}

/*
void	NodeShape2d::setScreenRect(const Rectf& bounds, const float depth)
{
	// compute affine transformation that maps the current screen Rect to the input screen Rect
	Rectf screen_rect = this->getScreenRect();
	float scale_x = bounds.getWidth() / screen_rect.getWidth();
	float scale_y = bounds.getHeight() / screen_rect.getHeight();
}
*/

Rectf NodeShape2d::getScreenRect(bool precise) const
{
	Rectf bounds = Node2d::getScreenRect(precise);
	
	if (precise) {
		bounds.include(mShape.calcPreciseBoundingBox().transformCopy(mWorldTransform));
	}
	else {
		bounds.include(mShape.calcBoundingBox().transformCopy(mWorldTransform));
	}
	
	return bounds;
}

Rectf NodeShape2d::getBounds() const
{
	Rectf boundary = Node2d::getBounds();
	Rectf shape_bounds = mShape.calcBoundingBox().transformCopy(mTransform);
	
	boundary.include(shape_bounds);
	
	return boundary;
}

Rectf NodeShape2d::getShapeBounds() const
{
	return mShape.calcBoundingBox().transformCopy(mTransform);
}

Vec2f NodeShape2d::getAnchorPercentage()
{
	Rectf shape_bounds = mShape.calcBoundingBox();
	float x = (mAnchor.x - shape_bounds.getX1()) / shape_bounds.getWidth();
	float y = (mAnchor.y - shape_bounds.getY1()) / shape_bounds.getHeight();
	return Vec2f(x,y);
}

bool NodeShape2d::mouseMove(MouseEvent event)
{
	// The event specifies the mouse coordinates in screen space, and our
	// node position is specified in parent space. So, transform coordinates
	// from one space to the other using the built-in methods.
	
	// check if mouse is inside node (screen space -> object space)
//	Vec2f o = Node2d::viewportToObject(event.getPos(), *this);
	Vec2f o = Vec2f(event.getPos());
	if (getScreenRect().contains(o)) {
		mStrokeColor = ColorA(0, 1, 0, 1);
		return true;
	} else {
		mStrokeColor = ColorA(1, 1, 1, 1);
		return false;
	}
}

bool NodeShape2d::mouseDown(MouseEvent event)
{
	// The event specifies the mouse coordinates in screen space, and our
	// node position is specified in parent space. So, transform coordinates
	// from one space to the other using the built-in methods.
	
	// check if we clicked inside node (screen space -> object space)
//	Vec2f o = Node2d::viewportToObject(event.getPos(), *this);
	Vec2f pos = Vec2f(event.getPos());
	if (!getScreenRect().contains(pos)) return false;
	
//	mFillColor = mFillSelectedColor;
	
	// calculate click offset
	if (mParent.lock()) {
		Vec2f local_position = Node2d::viewportToObject(pos, static_cast<const Node2d&>(*(mParent.lock().get())));		
		mMouseOffset = local_position - getPosition();
	}
	else {
		mMouseOffset = getPosition();
	}
	mIsDragged = true;
	
	return true;
}

bool NodeShape2d::mouseDrag(MouseEvent event)
{
	if (!mIsDragged) return false;

	if (mParent.lock()) {
		Vec2f local_position = Node2d::viewportToObject(Vec2f(event.getPos()), static_cast<const Node2d&>(*(mParent.lock().get())));
		Vec2f p = local_position - mMouseOffset;
		setPosition(p);
	}
	else {
		setPosition(mMouseOffset);
	}
	
	return true;
}

bool NodeShape2d::mouseUp(MouseEvent event)
{
	mIsDragged = false;
	
//	mFillColor = mFillUnselectedColor;
	
	return false;
}
