// TypeSystem.cpp

#include "stdafx.h"
#include "TypeSystem.h"

RTTypes* RTTypes::_theInstance;

GC_Object* RTTypes::CreateFromFile(ObjectType type)
{
	FromFileMap::const_iterator it = _ffm.find(type);
	if( _ffm.end() == it )
		return NULL;
	return it->second();
}

GC_Object* RTTypes::CreateObject(ObjectType type, float x, float y)
{
	assert(IsRegistered(type));
	return GetTypeInfo(type).Create(x, y);
}



// end of file
