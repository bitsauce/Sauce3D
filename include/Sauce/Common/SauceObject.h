// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>

BEGIN_SAUCE_NAMESPACE

struct SAUCE_API SauceObjectDesc
{
	virtual ~SauceObjectDesc() {}
};

class SAUCE_API SauceObject
{
public:
	virtual ~SauceObject() {}
	virtual bool initialize(SauceObjectDesc* objectDesc) = 0;
};

template<typename T, typename Enabled = void>
struct HasCreateImpl
{
	static constexpr bool value = false;
};

template<typename T>
struct HasCreateImpl<T, std::enable_if_t<std::is_invocable_r_v<T*, decltype(&T::CreateImpl)>>>
{
	static constexpr bool value = true;
};

template<typename T>
typename T::RefType CreateNew(const typename T::DescType& desc)
{
	static_assert(std::is_base_of<SauceObject, T>::value == true, "Template type passed to CreateNew<T>() must derive from SauceObject.");

	T* newObject;
	if constexpr (HasCreateImpl<T>::value)
	{
		newObject = T::CreateImpl();
		THROW_IF(newObject == nullptr, "CreateImpl() must return a valid object");
	}
	else
	{
		newObject = new T();
	}

	if (!newObject->initialize(desc))
	{
		delete newObject;
		return nullptr;
	}
	return T::RefType(newObject);
}

template<typename T, typename U>
T* sauce_cast(U object)
{
	return dynamic_cast<T*>(object.get());
}

END_SAUCE_NAMESPACE
