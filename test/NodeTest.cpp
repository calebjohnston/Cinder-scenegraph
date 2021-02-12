#include <string>
#include <map>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <stack>
#include <set>

#include "CinderGTest.h"

#include "Node.h"
#include "Node2d.h"
#include "Node3d.h"

using namespace ci;
using namespace cg;

///////////////////////////////////////////////////////////////////////////
//
// TODO:	
//
///////////////////////////////////////////////////////////////////////////

class NodeTest : public testing::Test {
public:
	NodeTest() : testing::Test() {
	}
	
	void SetUp()
	{
	}
	
	void TearDown()
	{
	}
	
protected:
	
	void create2dNodes() {
		mEmptyNode2 = Node2d::create();
		mEmptyNamedNode2 = Node2d::create("furst");
		mEmptyInvisibleNode2 = Node2d::create("second", false);
		mEmptyNonInteractiveNode2 = Node2d::create("third", true);
		mRootNode2 = Node2d::create("root2d");
	}
	
	void create3dNodes() {
		mEmptyNode3 = Node3d::create();
		mEmptyNamedNode3 = Node3d::create("1st");
		mEmptyInvisibleNode3 = Node3d::create("2nd", false);
		mEmptyNonInteractiveNode3 = Node3d::create("3rd", true);
		mRootNode3 = Node3d::create("root3d");
	}
	
	Node2dRef mEmptyNode2;
	Node2dRef mEmptyNamedNode2;
	Node2dRef mEmptyInvisibleNode2;
	Node2dRef mEmptyNonInteractiveNode2;
	Node2dRef mRootNode2;
	// copies
	Node2dRef mEmptyNode2Copy;
	Node2dRef mEmptyNamedNode2Copy;
	Node2dRef mEmptyInvisibleNode2Copy;
	Node2dRef mEmptyNonInteractiveNode2Copy;
	Node2dRef mRootNode2Copy;
	
	Node3dRef mEmptyNode3;
	Node3dRef mEmptyNamedNode3;
	Node3dRef mEmptyInvisibleNode3;
	Node3dRef mEmptyNonInteractiveNode3;
	Node3dRef mRootNode3;
	// copies
	Node3dRef mEmptyNode3Copy;
	Node3dRef mEmptyNamedNode3Copy;
	Node3dRef mEmptyInvisibleNode3Copy;
	Node3dRef mEmptyNonInteractiveNode3Copy;
	Node3dRef mRootNode3Copy;
	
};

TEST_F( NodeTest, ConstructionTest )
{
	EXPECT_NO_THROW(Node2d::create());
	EXPECT_NO_THROW(Node2d::create("furst"));
	EXPECT_NO_THROW(Node2d::create("second", false));
	EXPECT_NO_THROW(Node2d::create("third", true));
	EXPECT_NO_THROW(Node2d::create("root2d"));
	EXPECT_NO_THROW(Node3d::create());
	EXPECT_NO_THROW(Node3d::create("1st"));
	EXPECT_NO_THROW(Node3d::create("2nd", false));
	EXPECT_NO_THROW(Node3d::create("3rd", true));
	EXPECT_NO_THROW(Node3d::create("root3d"));
	
	create2dNodes();
		
	EXPECT_FALSE(mEmptyInvisibleNode2->isActive());
	EXPECT_TRUE(mEmptyNonInteractiveNode2->isActive());
	EXPECT_TRUE(mEmptyNode2->isActive());
	EXPECT_TRUE(mEmptyNamedNode2->isActive());
	EXPECT_EQ(mEmptyNamedNode2->getName(), "furst");
	EXPECT_EQ(mEmptyInvisibleNode2->getName(), "second");
	EXPECT_NE(mEmptyNode2->getName(), "second");
	
	create3dNodes();
		
	EXPECT_FALSE(mEmptyInvisibleNode3->isActive());
	EXPECT_TRUE(mEmptyNonInteractiveNode3->isActive());
	EXPECT_TRUE(mEmptyNode3->isActive());
	EXPECT_TRUE(mEmptyNamedNode3->isActive());
	EXPECT_EQ(mEmptyNamedNode3->getName(), "1st");
	EXPECT_EQ(mEmptyInvisibleNode3->getName(), "2nd");
	EXPECT_NE(mEmptyNode3->getName(), "2nd");
}

TEST_F( NodeTest, DestructionTest )
{
	Node2dRef emptyNode2Ref = Node2d::create();
	Node2dRef emptyNamedNode2Ref = Node2d::create("one");
	Node2dRef emptyInvisibleNode2Ref = Node2d::create("two", false);
	Node2dRef emptyNonInteractiveNode2Ref = Node2d::create("three", true);
	Node2dRef rootNode2Ref = Node2d::create();

	rootNode2Ref->addChild(emptyNode2Ref);
	rootNode2Ref->addChild(emptyNamedNode2Ref);
	rootNode2Ref->addChild(emptyInvisibleNode2Ref);
	rootNode2Ref->addChild(emptyNonInteractiveNode2Ref);
	EXPECT_NO_THROW(emptyNode2Ref.reset());
	EXPECT_NO_THROW(emptyNamedNode2Ref.reset());
	EXPECT_NO_THROW(emptyInvisibleNode2Ref.reset());
	EXPECT_NO_THROW(emptyNonInteractiveNode2Ref.reset());
	EXPECT_NO_THROW(rootNode2Ref.reset());
	
	const Node2d* node2d_null_ptr = NULL;
	EXPECT_EQ(emptyNode2Ref.get(), node2d_null_ptr);
	EXPECT_EQ(emptyNamedNode2Ref.get(), node2d_null_ptr);
	EXPECT_EQ(emptyInvisibleNode2Ref.get(), node2d_null_ptr);
	EXPECT_EQ(emptyNonInteractiveNode2Ref.get(), node2d_null_ptr);
	
	Node3dRef emptyNode3Ref = Node3d::create();
	Node3dRef emptyNamedNode3Ref = Node3d::create("one");
	Node3dRef emptyInvisibleNode3Ref = Node3d::create("two", false);
	Node3dRef emptyNonInteractiveNode3Ref = Node3d::create("three", true);
	Node3dRef rootNode3Ref = Node3d::create();
	rootNode3Ref->addChild(Node3dRef(emptyNode3Ref));
	rootNode3Ref->addChild(Node3dRef(emptyNamedNode3Ref));
	rootNode3Ref->addChild(Node3dRef(emptyInvisibleNode3Ref));
	rootNode3Ref->addChild(Node3dRef(emptyNonInteractiveNode3Ref));
	EXPECT_NO_THROW(emptyNode3Ref.reset());
	EXPECT_NO_THROW(emptyNamedNode3Ref.reset());
	EXPECT_NO_THROW(emptyInvisibleNode3Ref.reset());
	EXPECT_NO_THROW(emptyNonInteractiveNode3Ref.reset());
	EXPECT_NO_THROW(rootNode3Ref.reset());
	
	const Node3d* node3d_null_Ref = NULL;
	EXPECT_EQ(emptyNode3Ref.get(), node3d_null_Ref);
	EXPECT_EQ(emptyNamedNode3Ref.get(), node3d_null_Ref);
	EXPECT_EQ(emptyInvisibleNode3Ref.get(), node3d_null_Ref);
	EXPECT_EQ(emptyNonInteractiveNode3Ref.get(), node3d_null_Ref);
}

TEST_F( NodeTest, ParentingTest )
{	
	// test parenting methods with Node2d ...
	mRootNode2 = Node2d::create();
	EXPECT_FALSE(mRootNode2->addChild(mEmptyNode3)) << "Must not be able to add a NULL child to a Node2d parent";
	create2dNodes();
	EXPECT_FALSE(mRootNode2->addChild(mRootNode2)) << "Must not be able to add a Node2d parent to itself.";
	EXPECT_FALSE(mRootNode2->removeChild(mEmptyNode2)) << "Should return false if the child is not able to be removed.";
	
	EXPECT_TRUE(mRootNode2->addChild(mEmptyNode2));
	EXPECT_TRUE(mRootNode2->addChild(mEmptyNamedNode2));
	EXPECT_TRUE(mRootNode2->addChild(mEmptyInvisibleNode2));
	EXPECT_TRUE(mRootNode2->addChild(mEmptyNonInteractiveNode2));
	EXPECT_EQ(mRootNode2->getChildren().size(),4);
	EXPECT_TRUE(mRootNode2->removeChild(mEmptyNode2));
	EXPECT_TRUE(mRootNode2->removeChild(mEmptyNamedNode2));
	EXPECT_TRUE(mRootNode2->removeChild(mEmptyInvisibleNode2));
	EXPECT_TRUE(mRootNode2->removeChild(mEmptyNonInteractiveNode2));
	EXPECT_FALSE(mRootNode2->hasChildren());
	
	EXPECT_TRUE(mEmptyNode2->addChild(mRootNode2));
	EXPECT_TRUE(mEmptyNode2->addChild(mEmptyNamedNode2));
	EXPECT_TRUE(mEmptyNode2->addChild(mEmptyInvisibleNode2));
	EXPECT_TRUE(mEmptyNode2->addChild(mEmptyNonInteractiveNode2));
	EXPECT_EQ(mEmptyNode2->getChildren().size(),4);
	EXPECT_TRUE(mEmptyNode2->hasChild(mRootNode2));
	mRootNode2->removeFromParent();
	EXPECT_FALSE(mEmptyNode2->hasChild(mRootNode2));
	mEmptyNamedNode2->removeFromParent();
	mEmptyInvisibleNode2->removeFromParent();
	mEmptyNonInteractiveNode2->removeFromParent();
	EXPECT_EQ(mEmptyNode2->getChildren().size(),0);
	
	// test parenting methods with Node3d ...
	mRootNode3 = Node3d::create();
	EXPECT_FALSE(mRootNode3->addChild(mEmptyNode3)) << "Must not be able to add a NULL child to a Node3d parent";
	create3dNodes();
	EXPECT_FALSE(mRootNode3->addChild(mRootNode3)) << "Must not be able to add a Node3d parent to itself.";
	
	EXPECT_TRUE(mRootNode3->addChild(mEmptyNode3));
	EXPECT_TRUE(mEmptyNode3->addChild(mEmptyNamedNode3));
	EXPECT_TRUE(mEmptyNamedNode3->addChild(mEmptyInvisibleNode3));
	EXPECT_TRUE(mEmptyInvisibleNode3->addChild(mEmptyNonInteractiveNode3));
	EXPECT_FALSE(mRootNode3->addChild(mEmptyNode3));
	
	EXPECT_EQ(mRootNode3->getChildren().size(), 1);
	EXPECT_EQ(mEmptyNonInteractiveNode3->getChildren().size(), 0);
	EXPECT_EQ(mEmptyNonInteractiveNode3->getParent(), mEmptyInvisibleNode3);
	EXPECT_EQ(mEmptyInvisibleNode3->getParent(), mEmptyNamedNode3);
	EXPECT_EQ(mEmptyNamedNode3->getParent(), mEmptyNode3);
	EXPECT_EQ(mEmptyNode3->getParent(), mRootNode3);
	
	EXPECT_TRUE(mRootNode3->addChild(mEmptyNamedNode3));
	EXPECT_TRUE(mRootNode3->addChild(mEmptyInvisibleNode3));
	EXPECT_TRUE(mRootNode3->addChild(mEmptyNonInteractiveNode3));
	EXPECT_NE(mEmptyNonInteractiveNode3->getParent(), mEmptyInvisibleNode3);
	EXPECT_NE(mEmptyInvisibleNode3->getParent(), mEmptyNamedNode3);
	EXPECT_NE(mEmptyNamedNode3->getParent(), mEmptyNode3);
	mRootNode3->removeChildren();
	EXPECT_FALSE(mRootNode3->hasChildren());
	
	
	EXPECT_NE(mEmptyNonInteractiveNode3->getParent(), mRootNode3);
	EXPECT_NE(mEmptyInvisibleNode3->getParent(), mRootNode3);
	EXPECT_NE(mEmptyNamedNode3->getParent(), mRootNode3);
}

TEST_F( NodeTest, CopyTest )
{
	// test copying functionality with Node3d instances ...
	const Node3d* node3d_null_ptr = NULL;
	EXPECT_EQ(mEmptyNode3.get(), node3d_null_ptr);
	EXPECT_EQ(mEmptyNamedNode3.get(), node3d_null_ptr);
	EXPECT_EQ(mEmptyInvisibleNode3.get(), node3d_null_ptr);
	EXPECT_EQ(mEmptyNonInteractiveNode3.get(), node3d_null_ptr);
	create3dNodes();
	
	EXPECT_EQ(mEmptyNode3Copy.get(), node3d_null_ptr);
	EXPECT_EQ(mEmptyNamedNode3Copy.get(), node3d_null_ptr);
	EXPECT_EQ(mEmptyInvisibleNode3Copy.get(), node3d_null_ptr);
	EXPECT_EQ(mEmptyNonInteractiveNode3Copy.get(), node3d_null_ptr);
	
	// perform copy...
	EXPECT_NO_THROW(mEmptyNode3Copy = mEmptyNode3);
	EXPECT_NO_THROW(mEmptyNamedNode3Copy = mEmptyNamedNode3);
	EXPECT_NO_THROW(mEmptyInvisibleNode3Copy = mEmptyInvisibleNode3);
	EXPECT_NO_THROW(mEmptyNonInteractiveNode3Copy = mEmptyNonInteractiveNode3);
	
	// verify the copy of internals
	EXPECT_NE(mEmptyNode3Copy.get(), node3d_null_ptr);
	EXPECT_NE(mEmptyNamedNode3Copy.get(), node3d_null_ptr);
	EXPECT_NE(mEmptyInvisibleNode3Copy.get(), node3d_null_ptr);
	EXPECT_NE(mEmptyNonInteractiveNode3Copy.get(), node3d_null_ptr);
	EXPECT_FALSE(mEmptyInvisibleNode3Copy->isActive());
	EXPECT_TRUE(mEmptyNonInteractiveNode3Copy->isActive());
	EXPECT_TRUE(mEmptyNode3Copy->isActive());
	EXPECT_TRUE(mEmptyNamedNode3Copy->isActive());
	EXPECT_EQ(mEmptyNamedNode3Copy->getName(), "1st");
	EXPECT_EQ(mEmptyInvisibleNode3Copy->getName(), "2nd");
	
	// test copying functionality with Node2d instances ...
	create2dNodes();
	EXPECT_TRUE(mRootNode2->addChild(mEmptyNode2));
	EXPECT_TRUE(mRootNode2->addChild(mEmptyNamedNode2));
	EXPECT_TRUE(mRootNode2->addChild(mEmptyInvisibleNode2));
	EXPECT_TRUE(mRootNode2->addChild(mEmptyNonInteractiveNode2));
	
	// perform copy...
	EXPECT_NO_THROW(mRootNode2Copy = Node2dRef(mRootNode2));
	
	// verify the copy of internals
	EXPECT_EQ(mRootNode2Copy.get(), mRootNode2.get());
	EXPECT_TRUE(mRootNode2Copy->hasChild(mEmptyNode2));
	EXPECT_TRUE(mRootNode2Copy->hasChild(mEmptyNamedNode2));
	EXPECT_TRUE(mRootNode2Copy->hasChild(mEmptyInvisibleNode2));
	EXPECT_TRUE(mRootNode2Copy->hasChild(mEmptyNonInteractiveNode2));
}

TEST_F( NodeTest, NodeMovementTest )
{
	create2dNodes();
	
	mRootNode2->addChild(mEmptyNode2);
	mRootNode2->addChild(mEmptyNamedNode2);
	mRootNode2->addChild(mEmptyInvisibleNode2);
	mRootNode2->addChild(mEmptyNonInteractiveNode2);
	
	EXPECT_TRUE(mEmptyNonInteractiveNode2->isOnTop());
	EXPECT_EQ(mEmptyNonInteractiveNode2->isOnTop(), mRootNode2->isOnTop(mEmptyNonInteractiveNode2));
	EXPECT_NE(mEmptyInvisibleNode2->isOnTop(), mRootNode2->isOnTop(mEmptyNonInteractiveNode2));
	EXPECT_FALSE(mRootNode2->isOnTop());
	EXPECT_FALSE(mEmptyNode2->isOnTop());
	EXPECT_FALSE(mRootNode2->isOnTop(mEmptyNode2));
	mEmptyNode2->moveToTop();
	EXPECT_TRUE(mEmptyNode2->isOnTop());
	EXPECT_TRUE(mRootNode2->isOnTop(mEmptyNode2));
	
	EXPECT_FALSE(mEmptyNode2->isOnBottom());
	EXPECT_FALSE(mRootNode2->isOnBottom(mEmptyNode2));
	mEmptyNode2->moveToBottom();
	EXPECT_TRUE(mEmptyNode2->isOnBottom());
	EXPECT_TRUE(mRootNode2->isOnBottom(mEmptyNode2));
}

CINDER_APP_GTEST( NodeTest, RendererGl )
	