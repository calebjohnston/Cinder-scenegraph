#pragma once

#include <string>
#include <iostream>
#include <memory>

namespace sg {

class SceneObject;
typedef std::shared_ptr<SceneObject> ObjectRef;				//!< A shared pointer to a Component instance
typedef std::shared_ptr<const SceneObject> ObjectConstRef;	//!< A shared pointer to a constant Component instance
typedef std::weak_ptr<SceneObject> ObjectWeakRef;			//!< A weak pointer to a Component instance

/**
 * @brief SceneObject is the abstract base class for all entities within the scenegraph system
 *
 * SceneObject encapsulates logic for serialization and deserialization
 *
 */
class SceneObject : public std::enable_shared_from_this<SceneObject> {
public:
	SceneObject(const std::string& name = "");
	
	virtual ~SceneObject();
	
	//! accessor method for the name property
	std::string getName() const { return mName; }
	
	virtual void serialize() const {};
	virtual void deserialize() {};
	
	// stream logging support
	friend std::ostream& operator<<(std::ostream& lhs, const SceneObject& rhs) {
		return lhs << "[SceneObject name=" << rhs.mName << "]";
	}
	
protected:
	//! accessor method for the name property
	std::string& name() { return mName; }
	
	std::string		mName;			//!< The string name used to uniquely identify the entity
	static uint64_t sNameNum;		//!< The running counter used to ensure all names are unique

	template<class T>
	std::shared_ptr<T> shared_from_base()
	{
		return std::static_pointer_cast<T>( shared_from_this() );
	}
	
	template<class T>
	std::shared_ptr<const T> shared_from_base() const
	{
		return std::static_pointer_cast<const T>( shared_from_this() );
	}
};

}
