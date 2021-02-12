#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "NodeBase.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ScenegraphApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	scene::NodeBase* mNode;
};

void ScenegraphApp::setup()
{
}

void ScenegraphApp::mouseDown( MouseEvent event )
{
}

void ScenegraphApp::update()
{
}

void ScenegraphApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( ScenegraphApp, RendererGl )
