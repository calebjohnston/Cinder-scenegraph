#include <string>
#include <map>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <stack>
#include <set>

#include "cinder/Area.h"
#include "cinder/Matrix.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"

#include "CinderGTest.h"

#include "Node.h"
#include "Node3d.h"

using namespace ci;
using namespace cg;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	
//
///////////////////////////////////////////////////////////////////////////

class Node3dTest : public testing::Test {
public:
	Node3dTest() : testing::Test() {
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
	
	static Node3dRef buildTree(uint32_t max_children, uint32_t max_depth, uint32_t max_size)
	{
		Node3dRef current_node;
		Node3dRef root = Node3dRef(new Node3d("root", false, false));
		current_node = root;
		std::queue<Node3dRef> queue;
		queue.push(current_node);
		std::stack<Node3dRef> stack;
		
		Node3dRef node;
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
				Vec3f scale = Rand::randVec3f() * s;
				Vec3f pos = Rand::randVec3f() * s;
				Vec3f axis = Rand::randVec3f().normalized();
				Vec3f anchor_pos = Rand::randVec3f() * s;
				
				name = "node-" + toString(scene_tree_size);
				Node3dRef node(new Node3d(name, false, false));
				node->setScale(scale);
				node->setRotation(Rand::randFloat(2 * M_PI), axis);
				node->setPosition(pos);
				node->setAnchor(anchor_pos);
				float det = node->getTransform().determinant();
				if (det != det || det == 0) {
					node->setScale(1.0f);
					node->setRotation(Quatf::identity());
					node->setPosition(0,0,0);
					node->setAnchor(0,0,0);
				}
				current_node->addChild(node);
				queue.push(node);
				scene_tree_size++;
			}
		}
		
		return root;
	}
	
	void buildTransforms() {
		float min_scale_factor = -1000.0f;
		float max_scale_factor = 1000.0f;
		int32_t min_viewport_width = 50;
		int32_t max_viewport_width = 5170;
		int32_t min_viewport_height = 50;
		int32_t max_viewport_height = 2160;
		
		uint32_t coordinate_spaces = mCoordTransformTestEntropy;
		mCoordinateSpaces.resize(coordinate_spaces);
		for (size_t i = 0; i < coordinate_spaces; ++i) {
			float s = Rand::randFloat(min_scale_factor, max_scale_factor);
			Vec3f scale = Rand::randVec3f() * s;
			Vec3f trans = Rand::randVec3f() * s;
			float rotation = Rand::randFloat(2 * M_PI);
			Vec3f axis = Rand::randVec3f().normalized();
			Matrix44f mtx;
			mtx.translate(trans);
			mtx.scale(scale);
			mtx.rotate(axis, rotation);
			float det = mtx.determinant();
			if (det != det || det == 0) {
				mtx.setToIdentity();
			}
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
	Node3dRef mRootNode;
	
	std::list<Area> mViewPorts;
	std::list<Matrix44f> mCoordinateSpaces;
	uint32_t mNumCoordTransformTests;
	uint32_t mCoordTransformTestEntropy;
	uint32_t mSceneTreeMaxDepth;
	uint32_t mSceneTreeMaxSize;
	uint32_t mNodeMaxChildren;
};

TEST_F( Node3dTest, ConstructionTest )
{
	// EXPECT_FALSE(mFsObserverRef);
	// mFsObserverRef = FileSystemObserver::create();
	// EXPECT_TRUE(mFsObserverRef);
	// EXPECT_EQ(mFsObserverRef->running(), false);
	// FileSystemObserverRef fs_ref1 = FileSystemObserver::create("");
	// EXPECT_TRUE(fs_ref1);
	// EXPECT_EQ(fs_ref1->running(), false);
	// FileSystemObserverRef fs_ref2 = FileSystemObserver::create("/usr/local/bin", 1.0f);
	// EXPECT_TRUE(fs_ref2);
	// EXPECT_EQ(fs_ref2->running(), false);
	// EXPECT_EQ(fs_ref2->rootPath(), fs::path("/usr/local/bin"));
	// EXPECT_EQ(fs_ref2->pollingFrequency(), 1.0f);
	// mFsObserverRef.reset();
}

TEST_F( Node3dTest, DestructionTest )
{
	//
}

TEST_F( Node3dTest, iteratorTest )
{
	// we use a set to collect the names (which we know are unique)
	std::set<std::string> nameSet;
	typedef std::set<std::string>::iterator setIter;
	uint32_t node_count = math<uint32_t>::pow(mNodeMaxChildren, mSceneTreeMaxDepth);
	node_count += 1; // we add one for the root node
	
	//// CONST DEPTH FIRST
	Node::ConstIter depthFirst_constIter = mRootNode->getConstIter(true);
	while (depthFirst_constIter.hasNext()) {
		Node3dConstRef child = depthFirst_constIter.next<const Node3d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	// currently we know how big the tree is based upon the max children and the tree depth...
	EXPECT_EQ(nameSet.size(), node_count);
	nameSet.clear();
	
	//// NON-CONST DEPTH FIRST
	Node::Iter depthFirst_iter = mRootNode->getIter(true);
	while (depthFirst_iter.hasNext()) {
		Node3dRef child = depthFirst_iter.next<Node3d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	EXPECT_EQ(nameSet.size(), node_count);
	nameSet.clear();
	
	//// CONST BREADTH FIRST
	Node::ConstIter breadthFirst_constIter = mRootNode->getConstIter(false);
	while (breadthFirst_constIter.hasNext()) {
		Node3dConstRef child = breadthFirst_constIter.next<const Node3d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	EXPECT_EQ(nameSet.size(), node_count);
	nameSet.clear();
	
	//// NON-CONST BREADTH FIRST
	Node::Iter breadthFirst_iter = mRootNode->getIter(false);
	while (breadthFirst_iter.hasNext()) {
		Node3dRef child = breadthFirst_iter.next<Node3d>();
		std::pair<setIter,bool> result = nameSet.insert(child->getName());
		EXPECT_TRUE(result.second);
	}
	EXPECT_EQ(nameSet.size(), node_count);
}

/*
ci::Vec3f unproject(const ci::Vec3f& pt, const ci::Matrix44f& composed_inverse, const ci::Area& viewport);
*/


TEST_F( Node3dTest, viewportToObjectTest )
{
	float max_x_value = -500.0f;
	float max_y_value = 500.0f;
	std::list<Matrix44f>::const_iterator mtx_itr = mCoordinateSpaces.begin();
	std::list<Area>::const_iterator vp_itr = mViewPorts.begin();
	uint32_t transforms_counter = 0;
	uint32_t viewport_counter = 0;
	uint32_t viewport_limit = mNumCoordTransformTests / mViewPorts.size();
	uint32_t transform_limit = mNumCoordTransformTests / mCoordinateSpaces.size();
	for (uint32_t i = 0; i < mNumCoordTransformTests; ++i) {
		Matrix44f transform = *mtx_itr;
		Area viewport = *vp_itr;
		float x = Rand::randFloat(max_x_value);
		float y = Rand::randFloat(max_y_value);
		
		Vec3f coord(x,y,0);
		Vec3f actual = Node3d::viewportToObject(coord.xy(), transform, viewport);
		
		////////////////////////////////////
		coord.y = (viewport.getHeight() - coord.y);
		coord.z = 0.0f;
		Vec3f p0 = Node3d::unproject(coord, transform, viewport);
		coord.z = 1.0f;
		Vec3f p1 = Node3d::unproject(coord, transform, viewport);
		float alpha = (p1.z == p0.z)? 0: (0.0f - p0.z) / (p1.z - p0.z);
		Vec3f expected = lerp(p0, p1, alpha);
		
		EXPECT_NEAR(actual.x, expected.x, 0.0001);
		EXPECT_NEAR(actual.y, expected.y, 0.0001);
		EXPECT_NEAR(actual.z, expected.z, 0.0001);
		
		if (transforms_counter >= transform_limit) mtx_itr++;
		if (viewport_counter >= viewport_limit) vp_itr++;
	}
}

TEST_F( Node3dTest, objectToViewportTest )
{
	float max_x_value = -1500.0f;
	float max_y_value = 1500.0f;
	float max_z_value = 1500.0f;
	std::list<Matrix44f>::const_iterator mtx_itr = mCoordinateSpaces.begin();
	std::list<Area>::const_iterator vp_itr = mViewPorts.begin();
	uint32_t transforms_counter = 0;
	uint32_t viewport_counter = 0;
	uint32_t viewport_limit = mNumCoordTransformTests / mViewPorts.size();
	uint32_t transform_limit = mNumCoordTransformTests / mCoordinateSpaces.size();
	for (uint32_t i = 0; i < mNumCoordTransformTests; ++i) {
		Matrix44f transform = *mtx_itr;
		Area viewport = *vp_itr;
		float x = Rand::randFloat(max_x_value);
		float y = Rand::randFloat(max_y_value);
		float z = Rand::randFloat(max_z_value);
		Vec4f coord(x,y,z,1.0f);
		
		////////////////////////////////////
		Vec4f b = transform * coord;
		if (b.w != 0.0f) b.w = 1.0f / b.w;
		b.x *= b.w;
		b.y *= b.w;
		b.z *= b.w;
		Vec2f expected;
		expected.x = viewport.getX1() + viewport.getWidth() * (b.x + 1.0f) / 2.0f;
		expected.y = viewport.getY1() + viewport.getHeight() * (1.0f - (b.y + 1.0f) / 2.0f);
		
		// compute the same result using the Node2d method
		Vec2f actual = Node3d::objectToViewport(coord.xyz(), transform, viewport);
		
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

TEST_F( Node3dTest, parentToObjectTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node::ConstIter depth_first_iter = mRootNode->getConstIter(true);
	while (depth_first_iter.hasNext()) {
		Node3dConstRef child = depth_first_iter.next<const Node3d>();
		if (child->hasParent()) {
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			float z = Rand::randFloat(min_value, max_value);
			Vec3f pt(x,y,z);
			Vec3f actual = Node3d::parentToObject(pt, *(child.get()));
			Matrix44f trans = child->getTransform();
			trans.invert();
			
			// disregard degenerate transforms
			float det = trans.determinant();
			if (det != det || det == 0) continue;
			
			Vec3f expected = trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
			EXPECT_NEAR(expected.z, actual.z, 0.0001f);
		}
	}
}

TEST_F( Node3dTest, objectToParentTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node::Iter depth_first_iter = mRootNode->getIter(true);
	while (depth_first_iter.hasNext()) {
		Node3dRef child = depth_first_iter.next<Node3d>();
		if (child->hasParent()) {
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			float z = Rand::randFloat(min_value, max_value);
			Vec3f pt(x,y,z);
			Vec3f actual = Node3d::objectToParent(pt, *(child.get()));
			Matrix44f trans = child->getTransform();
			
			// disregard degenerate transforms
			float det = trans.determinant();
			if (det != det || det == 0) continue;
			
			Vec3f expected = trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
			EXPECT_NEAR(expected.z, actual.z, 0.0001f);
		}
	}
}

TEST_F( Node3dTest, worldToObjectTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node::Iter depth_first_iter = mRootNode->getIter(true);
	while (depth_first_iter.hasNext()) {
		Node3dRef child = depth_first_iter.next<Node3d>();
		if (child->hasParent()) {
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			float z = Rand::randFloat(min_value, max_value);
			Vec3f pt(x,y,z);
			Vec3f actual = Node3d::worldToObject(pt, *(child.get()));
			Matrix44f inv_trans = child->getWorldTransform().inverted();
			
			// disregard degenerate transforms
			float det = inv_trans.determinant();
			if (det != det || det == 0) continue;
			
			Vec3f expected = inv_trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
			EXPECT_NEAR(expected.z, actual.z, 0.0001f);
		}
	}
}

TEST_F( Node3dTest, objectToWorldTest )
{
	float min_value = -1000.0f;
	float max_value = 1000.0f;
	Node::Iter depth_first_iter = mRootNode->getIter(true);
	while (depth_first_iter.hasNext()) {
		Node3dRef child = depth_first_iter.next<Node3d>();
		if (child->hasParent()) {
			float x = Rand::randFloat(min_value, max_value);
			float y = Rand::randFloat(min_value, max_value);
			float z = Rand::randFloat(min_value, max_value);
			Vec3f pt(x,y,z);
			Vec3f actual = Node3d::objectToWorld(pt, *(child.get()));
			Matrix44f trans = child->getWorldTransform();
			
			// disregard degenerate transforms
			float det = trans.determinant();
			if (det != det || det == 0) continue;
			
			Vec3f expected = trans * pt;
			
			EXPECT_NEAR(expected.x, actual.x, 0.0001f);
			EXPECT_NEAR(expected.y, actual.y, 0.0001f);
			EXPECT_NEAR(expected.z, actual.z, 0.0001f);
		}
	}
}


/*

bool sortPositionX(const NodeRef& lhs, const NodeRef& rhs);

bool sortPositionY(const NodeRef& lhs, const NodeRef& rhs);

bool sortPositionZ(const NodeRef& lhs, const NodeRef& rhs);

bool sortBySize(const NodeRef& lhs, const NodeRef& rhs);
*/

CINDER_APP_GTEST( Node3dTest, RendererGl )
	