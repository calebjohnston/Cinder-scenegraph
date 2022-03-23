#include <functional>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "NodeBase.h"
#include "Node2d.h"
#include "Node3d.h"
#include "EventDispatcher.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

template<typename T, typename... U>
size_t get_address(std::function<T(U...)> f) {
	typedef T(fnType)(U...);
	fnType** fnPointer = f.template target<fnType*>();
	return (size_t) *fnPointer;
}

void handler0( sg::TestEvent event )
{
	std::cout << "handler0" << std::endl;
}

class ScenegraphTestApp : public App {
  public:
	
	
	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );
	
	void resize() {}
	
	void setup();
	void update();
	void draw();
	
protected:
	//! Keeps track of current game time, used to calculate elapsed time in seconds.
	double				mTime;
	
	sg::Node2dRef		mRoot;
	
	sg::Signal<sg::TestEvent> mSignalTest;
	
	void handler1( sg::TestEvent event ) { std::cout << "handler1" << std::endl; }
	
	virtual void handler2( sg::TestEvent event ) { std::cout << "handler2" << std::endl; }
	
	void handler3( sg::TestEvent event ) const { std::cout << "handler3" << std::endl; }
	
	void handler4( const sg::TestEvent event ) { std::cout << "handler4" << std::endl; }
	
	void handler5( const sg::TestEvent& event ) { std::cout << "handler5" << std::endl; }
	
	void handler6( const sg::TestEvent* event ) { std::cout << "handler6" << std::endl; }
};

void ScenegraphTestApp::setup()
{
	using namespace sg;
	std::function<void(TestEvent)> fn0 = std::bind(&handler0, std::placeholders::_1);
	std::function<void(TestEvent)> fn1 = std::bind(&ScenegraphTestApp::handler1, this, std::placeholders::_1);
	std::function<void(TestEvent)> fn2 = std::bind(&ScenegraphTestApp::handler2, this, std::placeholders::_1);
	std::function<void(TestEvent)> fn3 = std::bind(&ScenegraphTestApp::handler3, this, std::placeholders::_1);
	std::function<void(TestEvent)> fn4 = std::bind(&ScenegraphTestApp::handler4, this, std::placeholders::_1);
	std::function<void(TestEvent&)> fn5 = std::bind(&ScenegraphTestApp::handler5, this, std::placeholders::_1);
	std::function<void(TestEvent*)> fn6 = std::bind(&ScenegraphTestApp::handler6, this, std::placeholders::_1);
	
	std::function<void(TestEvent)> fn0_2 = std::bind(&handler0, std::placeholders::_1);
	std::function<void(TestEvent)> fn1_2 = std::bind(&ScenegraphTestApp::handler1, this, std::placeholders::_1);
	std::function<void(TestEvent)> fn2_2 = std::bind(&ScenegraphTestApp::handler2, this, std::placeholders::_1);
	std::function<void(TestEvent)> fn3_2 = std::bind(&ScenegraphTestApp::handler3, this, std::placeholders::_1);
	std::function<void(TestEvent)> fn4_2 = std::bind(&ScenegraphTestApp::handler4, this, std::placeholders::_1);
	std::function<void(TestEvent&)> fn5_2 = std::bind(&ScenegraphTestApp::handler5, this, std::placeholders::_1);
	std::function<void(TestEvent*)> fn6_2 = std::bind(&ScenegraphTestApp::handler6, this, std::placeholders::_1);
	
	fn0(TestEvent());
	fn1(TestEvent());
	fn2(TestEvent());
	fn3(TestEvent());
	fn4(TestEvent());
	
	TestEvent e1 = TestEvent();
	fn5(e1);
	fn6(&e1);
	
	fn0_2(TestEvent());
	fn1_2(TestEvent());
	fn2_2(TestEvent());
	fn3_2(TestEvent());
	fn4_2(TestEvent());
	
	TestEvent e2 = TestEvent();
	fn5_2(e2);
	fn6_2(&e2);
//	size_t fn1_addr = get_address(fn1);
//	size_t fn1_2_addr = get_address(fn1_2);
//	std::cout << "fn0: " << fn1_addr << " ==? " << fn1_2_addr << std::endl;
//	std::cout << "fn1: " << get_address(fn1) << " ==? " << get_address(fn1_2) << std::endl;
//	std::cout << "fn2: " << get_address(fn2) << " ==? " << get_address(fn2_2) << std::endl;
//	std::cout << "fn3: " << get_address(fn3) << " ==? " << get_address(fn3_2) << std::endl;
//	std::cout << "fn4: " << get_address(fn4) << " ==? " << get_address(fn4_2) << std::endl;
//	std::cout << "fn5: " << get_address(fn5) << " ==? " << get_address(fn5_2) << std::endl;
//	std::cout << "fn6: " << get_address(fn6) << " ==? " << get_address(fn6_2) << std::endl;
	
	// Initialize game time
	mTime = getElapsedSeconds();
	
	// create the root node: a large rectangle
	mRoot = sg::Node2d::create();
	// specify the position of the anchor point on our canvas
	mRoot->setPosition(320, 240);
	/*
	// set the size of the node
	mRoot->setSize(600, 450);
	// we can easily set the anchor point to its center:
	mRoot->setPivotPercentage(0.5f, 0.5f);
	
	// add smaller rectangles to the root node
	Node2dRef child1 = sg::Node2d::create();
	child1->setPosition(0, 0);
	child1->setSize(240, 200);
	mRoot->addChild(child1);
	
	Node2dRef child2 = sg::Node2d::create();
	child2->setPosition(260, 0);
	child2->setSize(240, 200);
	mRoot->addChild(child2);
	
	// add even smaller rectangles to the child rectangles
	Node2dRef child = sg::Node2d::create();
	child->setPosition(5, 5);
	child->setSize(100, 100);
	child1->addChild(child);
	
	//child.reset( new NodeRectangle() );
	child->setPosition(5, 5);
	child->setSize(100, 100);
	child2->addChild(child);
	 */
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


CINDER_APP( ScenegraphTestApp, RendererGl )
