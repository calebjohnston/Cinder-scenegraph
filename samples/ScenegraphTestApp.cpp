#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "NodeBase.h"
#include "Node2d.h"
#include "Node3d.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ScenegraphTestApp : public App {
  public:
	
	
	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );
	
	void resize();
	
	void setup();
	void update();
	void draw();
	
protected:
	//! Keeps track of current game time, used to calculate elapsed time in seconds.
	double				mTime;
	
	scene::Node2dRef		mRoot;
};

void ScenegraphTestApp::setup()
{
	// Initialize game time
	mTime = getElapsedSeconds();
	
	// create the root node: a large rectangle
	mRoot = scene::Node2d::create();
	// specify the position of the anchor point on our canvas
	mRoot->setPosition(320, 240);
	// set the size of the node
	mRoot->setSize(600, 450);
	// we can easily set the anchor point to its center:
	mRoot->setPivotPercentage(0.5f, 0.5f);
	
	// add smaller rectangles to the root node
	Node2dRef child1 = scene::Node2d::create();
	child1->setPosition(0, 0);
	child1->setSize(240, 200);
	mRoot->addChild(child1);
	
	Node2dRef child2 = scene::Node2d::create();
	child2->setPosition(260, 0);
	child2->setSize(240, 200);
	mRoot->addChild(child2);
	
	// add even smaller rectangles to the child rectangles
	Node2dRef child = scene::Node2d::create();
	child->setPosition(5, 5);
	child->setSize(100, 100);
	child1->addChild(child);
	
	//child.reset( new NodeRectangle() );
	child->setPosition(5, 5);
	child->setSize(100, 100);
	child2->addChild(child);
}

void ScenegraphTestApp::mouseMove( MouseEvent event )
{
	// pass the mouseMove event to all nodes. Important: this can easily bring your
	// frame rate down if you have a lot of nodes and none of them does anything with
	// this event. Only use it if you must! Needs optimization.
//	mRoot->deepMouseMove(event);
}

void ScenegraphTestApp::mouseDown( MouseEvent event )
{
	// pass the mouseDown event to all nodes. This is usually very quick because
	// it starts at the top nodes and they often catch the event.
//	mRoot->deepMouseDown(event);
}

void ScenegraphTestApp::mouseDrag( MouseEvent event )
{
	// pass the mouseDrag event to all nodes. This is usually very quick.
//	mRoot->deepMouseDrag(event);
}

void ScenegraphTestApp::mouseUp( MouseEvent event )
{
	// pass the mouseUp event to all nodes. This is usually very quick.
//	mRoot->deepMouseUp(event);
}

void ScenegraphTestApp::keyDown( KeyEvent event )
{
	// let nodes handle keys first
	/*
	if (!mRoot->deepKeyDown(event)) {
		switch(event.getCode()) {
			case KeyEvent::KEY_ESCAPE:
				quit();
				break;
			case KeyEvent::KEY_f:
				setFullScreen(!isFullScreen());
				break;
		}
	}
	*/
}

void ScenegraphTestApp::keyUp( KeyEvent event )
{
//	mRoot->deepKeyUp(event);
}

void ScenegraphTestApp::update()
{
	// calculate elapsed time since last frame
	double elapsed = getElapsedSeconds() - mTime;
	mTime = getElapsedSeconds();
	
	// rotate the root node around its anchor point
	mRoot->setRotation( 0.1 * mTime );
	
	// update all nodes
	mRoot->deepUpdate( elapsed );
	
	// important and easy to forget: calculate transformations of all nodes
	// after they have been updated, so the transformation matrices reflect
	// any animation done on the nodes
	mRoot->deepTransform( gl::getModelView() );
}

void ScenegraphTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	// draw all nodes, starting with the root node
	mRoot->deepDraw();
}


CINDER_APP_BASIC( ScenegraphTestApp, RendererGl )
