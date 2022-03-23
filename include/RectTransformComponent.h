#pragma once

#include <string>
#include <iostream>
#include <memory>

#include "ComponentBase.hpp"
#include "ComponentFactory.h"

namespace sg {

class RectTransformComponent;
typedef std::shared_ptr<RectTransformComponent> RectTransformRef;				//!< A shared pointer to a TransformComponent instance
typedef std::shared_ptr<const RectTransformComponent> RectTransformConstRef;	//!< A shared pointer to a constant TransformComponent instance
typedef std::weak_ptr<RectTransformComponent> RectTransformWeakRef;				//!< A weak pointer to a TransformComponent instance

/**
 * @brief Component is an abstract base class for components added to the scenegraph
 *
 */
class RectTransformComponent : public sg::ComponentBase {
public:
	virtual ~RectTransformComponent();
	
	/** @inherit */
	virtual void setup() override;
	
	/** @inherit */
	virtual void update( double elapsed ) override;
	
	// stream logging support
	friend std::ostream& operator<<(std::ostream& lhs, const RectTransformComponent& rhs) {
		return lhs << "[RectTransformComponent name=" << rhs.mName << "]";
	}
	
protected:
	friend class RectTransformFactory;

	bool				mTransformIsDirty;	//!< flag used to manage transformation matrix cache
	ci::vec2			mPosition;			//!< the 2D or 3D position of the node
	ci::vec2			mScale;				//!< the 2D or 3D scale applied to the node
	ci::vec2			mPivot;				//!< the 2D or 3D location about which the node rotates
	float				mRotation;			//!< the floating point rotation represented in radians
	ci::mat3			mTransform;			//!< represents local transformation
	ci::mat3			mWorldTransform;	//!< represents world transformation
	
	RectTransformComponent(const std::string& name = "RectTransformComponent", const ci::mat3& transform = ci::mat3(1));
};

class RectTransformFactory : public sg::ComponentFactory {
public:
	RectTransformFactory();
	virtual ~RectTransformFactory();
	
	virtual void setup() override;
	
	virtual void update( double elapsed ) override;
	
	ComponentRef create(const std::string& name, const ci::mat3& transform = ci::mat3(1));
	
	// TEST...
	virtual ComponentRef create(const std::string& name) override;
	
protected:
	
};
	
}
