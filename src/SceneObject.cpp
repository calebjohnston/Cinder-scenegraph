#include <string>
#include <iostream>
#include <memory>

#include "Utils.hpp"
#include "SceneObject.h"

using namespace sg;

uint64_t SceneObject::sNameNum = 0;

SceneObject::SceneObject(const std::string& name)
{
	mName = name + "_" + sg::toFormattedString(sNameNum++, 8);
}

SceneObject::~SceneObject()
{
}

