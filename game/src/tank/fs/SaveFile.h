// SaveFile.h

#pragma once

#include "FileSystem.h"
#include "gc/Object.h"

class SaveFile
{
	typedef std::map<GC_Object*, size_t> PtrToIndex;
	typedef std::vector<GC_Object*> IndexToPtr;

	PtrToIndex _ptrToIndex;
	IndexToPtr _indexToPtr;

	SafePtr<FS::Stream> _stream;
	bool _load;

public:
	SaveFile(const SafePtr<FS::Stream> &s, bool loading);

	bool loading() const
	{
		return _load;
	}

	FS::Stream* GetStream() const { return _stream; }

	void Serialize(std::string &str);

	template<class T>
	void Serialize(T &value);

	template<class T>
	void Serialize(ObjPtr<T> &ptr);

	template<class T>
	void SerializeArray(T *p, size_t count);

	void RegPointer(GC_Object *ptr);
	size_t GetPointerId(GC_Object *ptr) const;
	GC_Object* RestorePointer(size_t id) const;

private:
	template<class T>
	void Serialize(const T &);
	template<class T>
	void Serialize(const ObjPtr<T> &);
	template<class T>
	void Serialize(T *) {assert(!"you are not allowed to serialize raw pointers");}
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
void SaveFile::Serialize(T &obj)
{
	assert(typeid(obj) != typeid(std::string));
	assert(typeid(obj) != typeid(std::string));
	assert(!strstr(typeid(obj).name(), "SafePtr"));
	assert(!strstr(typeid(obj).name(), "ObjPtr"));
	if( loading() )
		_stream->Read(&obj, sizeof(T));
	else
		_stream->Write(&obj, sizeof(T));
}

template<class T>
void SaveFile::Serialize(ObjPtr<T> &ptr)
{
	size_t id;
	if( loading() )
	{
		Serialize(id);
		if( id )
		{
			GC_Object *raw = RestorePointer(id);
			if( !dynamic_cast<T*>(raw) )
				throw std::runtime_error("ERROR: invalid object pointer");
			ptr = static_cast<T*>(raw);
		}
		else
		{
			ptr = NULL;
		}
	}
	else
	{
		id = GetPointerId(ptr);
		Serialize(id);
	}
}

template<class T>
void SaveFile::SerializeArray(T *p, size_t count)
{
	assert(typeid(T) != typeid(std::string));
	assert(typeid(T) != typeid(std::string));
	assert(!strstr(typeid(T).name(), "SafePtr"));
	assert(!strstr(typeid(T).name(), "RawPtr"));
	if( loading() )
		_stream->Read(p, sizeof(T) * count);
	else
		_stream->Write(p, sizeof(T) * count);
}

// end of file
