#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class scenegraphApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void scenegraphApp::setup()
{
}

void scenegraphApp::mouseDown( MouseEvent event )
{
}

void scenegraphApp::update()
{
}

void scenegraphApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( scenegraphApp, RendererGl )
