#pragma once

#include <vector>
#include <string>
#include "value.h"

using json = Value;

//*********************************************************************
// from_json
//*********************************************************************
// array嵌套反序列化
template <typename T>
void from_json(json &j, std::vector<T> &arr)
{
	for (auto &v : j.get_array())
	{
		T obj;
		from_json(v, obj);
		arr.emplace_back(obj);
	}
}

void from_json(json &j, int &i);
// 实数反序列化
void from_json(json &j, double &d);
// bool反序列化
void from_json(json &j, bool &b);
//  std::string反序列化
void from_json(json &j, std::string &b);

//*********************************************************************
// User APIs
//*********************************************************************
// get value with key-value exisitence check
template <typename T>
void jfind(T &obj, json &data, const char *search_key)
{
	if (data.has_member(search_key))
		from_json(data[search_key], obj);
}

template <typename T>
void jfind(T *&obj, json &data, const char *search_key)
{
	if (data.has_member(search_key))
	{
		if (obj == nullptr)
			obj = new T();

		from_json(data[search_key], *obj);
	}
}

template <typename T>
void jfind_recursive(T &obj, json &data, const char *search_key)
{
	bool valid = true;
	json *tmp = &data;
	const char delim = '.';
	char *str = (char *)search_key;
	char *pos = (char *)std::strchr(str, delim);
	while (pos != nullptr)
	{
		FRString key(str, pos - str);
		if (tmp->has_member(key))
		{
			tmp = tmp->get_value(key);
		}
		else
		{
			valid = false;
			break;
		}
		str = ++pos;
		pos = (char *)std::strchr(str, delim);
	}
	if (valid)
		from_json(*tmp, obj);
}

// assert key exists
template <typename T>
inline void jget(T &obj, json &data, const char *search_key)
{
	from_json(data[search_key], obj);
}

template <typename T>
inline void jget_ptr(T *&obj, json &data, const char *search_key)
{
	if (obj == nullptr)
		obj = new T();

	from_json(data[search_key], *obj);
}

template <typename T>
void jget_recursive(T &obj, json &data, const char *search_key)
{
	json *tmp = &data;
	const char delim = '.';
	char *str = (char *)search_key;
	char *pos = (char *)std::strchr(str, delim);
	while (pos != nullptr)
	{
		FRString key(str, pos - str);
		tmp = tmp->get_value(key);
		str = ++pos;
		pos = (char *)std::strchr(str, delim);
	}
	from_json(*tmp, obj);
}

template <typename T>
inline void jdecode_ptr(T *&obj, json &data)
{
	if (obj == nullptr)
		obj = new T();

	from_json(data, *obj);
}
