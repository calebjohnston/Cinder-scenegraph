#include <string>
#include <map>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <stack>
#include <set>

#include "cinder/Area.h"
#include "cinder/MatrixAffine2.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"

#include "CinderGTest.h"
#include "getmem.h"

#include "Node.h"
#include "Node2d.h"

using namespace ci;
using namespace cg;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	
//
///////////////////////////////////////////////////////////////////////////

class Node2dTest : public testing::Test {
public:
	Node2dTest() : testing::Test() {
	}
	
	void SetUp()
	{
		Rand::randSeed(0xff);
		mNumCoordTransformTests = 1000;
		mCoordTransformTestEntropy = mNumCoordTransformTests / 10;
		mSceneTreeMaxDepth = 5;
		mNodeMaxChildren = 5;
		mSceneTreeMaxSize = math<uint32_t>::pow(mNodeMaxChildren, mSceneTreeMaxDepth);
		
		mRootNode = buildTree(mNodeMaxChildren, mSceneTreeMaxDepth, mSceneTreeMaxSize);
		// we must compose world transforms before performing coordinate space transformations...
		mRootNode->deepTransform();
		
		// create set of transforms, viewports, etc...
		buildTransforms();
	}
	
	void TearDown()
	{
	}
	
	static Node2dRef buildTree(uint32_t max_children, uint32_t max_depth, uint32_t max_size)
	{
		Node2dRef current_node;
		Node2dRef root = Node2dRef(new Node2d("root", false, false));
		current_node = root;
		std::queue<Node2dRef> queue;
		queue.push(current_node);
		std::stack<Node2dRef> stack;
		
		Node2dRef node;
		std::string name;
		float min_scale_factor = -1000.0f;
		float max_scale_factor = 1000.0f;
		uint32_t scene_tree_size;
		uint32_t total_children = math<uint32_t>::pow(max_children, max_depth);
		uint32_t max_tree_size = math<uint32_t>::min(max_size, total_children);
		for (scene_tree_size = queue.size(); !queue.empty(); ) {
			if (stack.size() >= max_depth) {
				current_node = stack.top();
				stack.pop();
			}
			else {
				current_node = queue.front();
				queue.pop();
				stack.push(current_node);
			}
			
			if (stack.empty()) break;
			
			if (scene_tree_size >= max_tree_size) break;
			
			while (current_node->getChildCount() < max_children) {
				float s = Rand::randFloat(min_scale_factor, max_scale_factor);
				Vec2f scale = Rand::randVec2f() * s;
				Vec2f pos = Rand::randVec2f() * s;
				Vec2f anchor_pos = Rand::randVec2f() * s;
				
				name = "node-" + toString(scene_tree_size);
				Node2dRef node(new Node2d(name, false, false));
				node->setScale(scale);
				node->setRotation(Rand::randFloat(2 * M_PI));
				node->setPosition(pos);
				node->setAnchor(anchor_pos);
				current_node->addChild(node);
				queue.push(node);
				scene_tree_size++;
			}
		}
		
		return root;
	}
	
	void buildTransforms() {
		float min_scale_factor = -5000.0f;
		float max_scale_factor = 5000.0f;
		int32_t min_viewport_width = 50;
		int32_t max_viewport_width = 5170;
		int32_t min_viewport_height = 50;
		int32_t max_viewport_height = 2160;
		
		uint32_t coordinate_spaces = mCoordTransformTestEntropy;
		mCoordinateSpaces.resize(coordinate_spaces);
		for (size_t i = 0; i < coordinate_spaces; ++i) {
			float s = Rand::randFloat(min_scale_factor, max_scale_factor);
			Vec2f scale = Rand::randVec2f() * s;
			Vec2f trans = Rand::randVec2f() * s;
			float rotation = Rand::randFloat(2 * M_PI);
			MatrixAffine2f mtx;
			mtx.translate(trans);
			mtx.scale(scale);
			mtx.rotate(rotation);
			mCoordinateSpaces.push_back(mtx);
		}
		
		uint32_t viewports = mCoordTransformTestEntropy;
		mViewPorts.resize(viewports);
		for (size_t i = 0; i < viewports; ++i) {
			Vec2i upper_left(Rand::randInt(min_viewport_width), Rand::randInt(min_viewport_height));
			Vec2i lower_right(Rand::randInt(max_viewport_width), Rand::randInt(max_viewport_height));
			Area a(upper_left, lower_right);
			mViewPorts.push_back(a);
		}
	}
	
protected:
	Node2dRef mRootNode;
	Node2dRef mChildNode;
	
	std::list<Area> mViewPorts;
	std::list<MatrixAffine2f> mCoordinateSpaces;
	uint32_t mNumCoordTransformTests;
	uint32_t mCoordTransformTestEntropy;
	uint32_t mSceneTreeMaxDepth;
	uint32_t mSceneTreeMaxSize;
	uint32_t mNodeMaxChildren;
};

TEST_F( Node2dTest, iteratorTest )
{
	// we use a set to collect the names (which we know are unique)
	std::set<std::string> nameSet;
	typedef std::set<std::string>::iterator setIter;
	uint32_t node_count = math<uint32_t>::pow(mNodeMaxChildren, mSceneTreeMaxDepth);
	node_count += 1; // we add one for the root node
	
	//// CONST DEPTH FIRST
	Node::ConstIter depthFirst_constIter = mRootNode->getConstIter(true);
	while (depthFirst_constIter.hasNext()) {
		Node2dConstRef child = depthFirst_constIter.next<const Node2d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	// currently we know how big the tree is based upon the max children and the tree depth...
	EXPECT_EQ(nameSet.size(), node_count);
	nameSet.clear();
	
	//// NON-CONST DEPTH FIRST
	Node::Iter depthFirst_iter = mRootNode->getIter(true);
	while (depthFirst_iter.hasNext()) {
		Node2dRef child = depthFirst_iter.next<Node2d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	EXPECT_EQ(nameSet.size(), node_count);
	nameSet.clear();
	
	//// CONST BREADTH FIRST
	Node::ConstIter breadthFirst_constIter = mRootNode->getConstIter(false);
	while (breadthFirst_constIter.hasNext()) {
		Node2dConstRef child = breadthFirst_constIter.next<const Node2d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	EXPECT_EQ(nameSet.size(), node_count);
	nameSet.clear();
	
	//// NON-CONST BREADTH FIRST
	Node::Iter breadthFirst_iter = mRootNode->getIter(false);
	while (breadthFirst_iter.hasNext()) {
		Node2dRef child = breadthFirst_iter.next<Node2d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	EXPECT_EQ(nameSet.size(), node_count);
}

TEST_F( Node2dTest, viewportToObjectTest )
{
	float max_x_value = -500.0f;
	float max_y_value = 500.0f;
	std::list<MatrixAffine2f>::const_iterator mtx_itr = mCoordinateSpaces.begin();
	std::list<Area>::const_iterator vp_itr = mViewPorts.begin();
	uint32_t transforms_counter = 0;
	uint32_t viewport_counter = 0;
	uint32_t viewport_limit = mNumCoordTransformTests / mViewPorts.size();
	uint32_t transform_limit = mNumCoordTransformTests / mCoordinateSpaces.size();
	for (uint32_t i = 0; i < mNumCoordTransformTests; ++i) {
		MatrixAffine2f transform = *mtx_itr;
		Area viewport = *vp_itr;
		float x = Rand::randFloat(max_x_value);
		float y = Rand::randFloat(max_y_value);
		Vec2f coord(x,y);
		Vec2f actual = Node2d::viewportToObject(coord, transform, viewport);
		Vec2f expected = transform.transformPoint(coord);
		
		EXPECT_NEAR(actual.x, expected.x, 0.0001);
		EXPECT_NEAR(actual.y, expected.y, 0.0001);
		
		if (transforms_counter >= transform_limit) {
			mtx_itr++;
		}
		if (viewport_counter >= viewport_limit) {
			vp_itr++;
		}
	}
}

TEST_F( Node2dTest, objectToViewportTest )
{
	float max_x_value = -1500.0f;
	float max_y_value = 1500.0f;
	std::list<MatrixAffine2f>::const_iterator mtx_itr = mCoordinateSpaces.begin();
	std::list<Area>::const_iterator vp_itr = mViewPorts.begin();
	uint32_t transforms_counter = 0;
	uint32_t viewport_counter = 0;
	uint32_t viewport_limit = mNumCoordTransformTests / mViewPorts.size();
	uint32_t transform_limit = mNumCoordTransformTests / mCoordinateSpaces.size();
	for (uint32_t i = 0; i < mNumCoordTransformTests; ++i) {
		MatrixAffine2f transform = *mtx_itr;
		Area viewport = *vp_itr;
		float x = Rand::randFloat(max_x_value);
		float y = Rand::randFloat(max_y_value);
		Vec2f coord(x,y);
		
		// compute transformation...
		Vec2f b = transform.transformPoint(coord);
		b.x = (b.x + 1.0f) / 2.0f;
		b.y = (1.0f - (b.y + 1.0f) / 2.0f);
		
		Vec2f expected;
		expected.x = viewport.getX1() + viewport.getWidth() * b.x;
		expected.y = viewport.getY1() + viewport.getHeight() * b.y;
		
		// compute the same result using the Node2d method
		Vec2f actual = Node2d::objectToViewport(coord, transform, viewport);
		
		EXPECT_NEAR(actual.x, expected.x, 0.0001);
		EXPECT_NEAR(actual.y, expected.y, 0.0001);
		
		if (transforms_counter >= transform_limit) {
			mtx_itr++;
		}
		if (viewport_counter >= viewport_limit) {
			vp_itr++;
		}
	}
}

TEST_F( Node2dTest, parentToObjectTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node2dConstRef parent;
	Node::ConstIter depth_first_iter = mRootNode->getConstIter(true);
	while (depth_first_iter.hasNext()) {
		Node2dConstRef child = depth_first_iter.next<const Node2d>();
		if (child->hasParent()) {
			parent = child->getParent<const Node2d>();
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			Vec2f pt(x,y);
			Vec2f actual = Node2d::parentToObject(pt, *(child.get()));
			MatrixAffine2f trans = child->getTransform();
			trans.invert();
			Vec2f expected = trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
		}
	}
}

TEST_F( Node2dTest, objectToParentTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node2dRef parent;
	Node::Iter depth_first_iter = mRootNode->getIter(true);
	while (depth_first_iter.hasNext()) {
		Node2dRef child = depth_first_iter.next<Node2d>();
		if (child->hasParent()) {
			parent = child->getParent<Node2d>();
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			Vec2f pt(x,y);
			Vec2f actual = Node2d::objectToParent(pt, *(child.get()));
			MatrixAffine2f trans = child->getTransform();
			Vec2f expected = trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
		}
	}
}

TEST_F( Node2dTest, worldToObjectTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node2dRef parent;
	Node::Iter depth_first_iter = mRootNode->getIter(true);
	while (depth_first_iter.hasNext()) {
		Node2dRef child = depth_first_iter.next<Node2d>();
		if (child->hasParent()) {
			parent = child->getParent<Node2d>();
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			Vec2f pt(x,y);
			Vec2f actual = Node2d::worldToObject(pt, *(child.get()));
			MatrixAffine2f trans = child->getWorldTransform();
			trans.invert();
			Vec2f expected = trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
		}
	}
}

TEST_F( Node2dTest, objectToWorldTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node2dRef parent;
	Node::Iter depth_first_iter = mRootNode->getIter(true);
	while (depth_first_iter.hasNext()) {
		Node2dRef child = depth_first_iter.next<Node2d>();
		if (child->hasParent()) {
			parent = child->getParent<Node2d>();
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			Vec2f pt(x,y);
			Vec2f actual = Node2d::objectToWorld(pt, *(child.get()));
			MatrixAffine2f trans = child->getWorldTransform();
			Vec2f expected = trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
		}
	}
}

CINDER_APP_GTEST( Node2dTest, RendererGl )
	