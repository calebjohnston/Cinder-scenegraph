#pragma once

#include <string>
#include <iostream>
#include <memory>

#include "ComponentBase.hpp"
#include "ComponentFactory.h"

namespace sg {

class TransformComponent;
typedef std::shared_ptr<TransformComponent> TransformRef;				//!< A shared pointer to a TransformComponent instance
typedef std::shared_ptr<const TransformComponent> TransformConstRef;	//!< A shared pointer to a constant TransformComponent instance
typedef std::weak_ptr<TransformComponent> TransformWeakRef;				//!< A weak pointer to a TransformComponent instance

/**
 * @brief Component is an abstract base class for components added to the scenegraph
 *
 */
class TransformComponent : public sg::ComponentBase {
public:
	virtual ~TransformComponent();
	
	/** @inherit */
	virtual void setup() override;
	
	/** @inherit */
	virtual void update( double elapsed ) override;
	
	// stream logging support
	friend std::ostream& operator<<(std::ostream& lhs, const TransformComponent& rhs) {
		return lhs << "[TransformComponent name=" << rhs.mName << "]";
	}
	
protected:
	friend class TransformFactory;
	
	bool				mTransformIsDirty;	//!< flag used to manage transformation matrix cache
	ci::vec3			mPosition;			//!< the 2D or 3D position of the node
	ci::vec3			mScale;				//!< the 2D or 3D scale applied to the node
	ci::vec3			mPivot;				//!< the 2D or 3D location about which the node rotates
	ci::vec3			mSize;				//!< the 2D or 3D size of the node (based upon contents)
	ci::quat			mRotation;			//!< the quaternion rotation applied to the node
	ci::mat4			mTransform;			//!< represents local transformation
	ci::mat4			mWorldTransform;	//!< represents world transformation
	
	TransformComponent(const std::string& name = "TransformComponent", const ci::mat4& transform = ci::mat4(1));
};

class TransformFactory : public sg::ComponentFactory {
public:
	TransformFactory();
	virtual ~TransformFactory();
	
	virtual void setup() override;
	
	virtual void update( double elapsed ) override;
	
	/**
	 * TODO: create a templatized version that can support arbitrary input parameters that are forwarded to the object constructor
	 */
	virtual ComponentRef create(const std::string& name, const ci::mat4& transform = ci::mat4(1));
	
	// TEST...
	virtual ComponentRef create(const std::string& name) override;
	
protected:
	
};
	
}
