#include "interface.h"
#include "string.h"

//*********************************************************************
// from_json
//*********************************************************************
// 整数反序列化
void from_json(json &j, int &i)
{
	i = (int)j.get_int();
}
// 实数反序列化
void from_json(json &j, double &d)
{
	d = j.get_double();
}
// bool反序列化
void from_json(json &j, bool &b)
{
	b = j.get_bool();
}
//  std::string反序列化
void from_json(json &j, std::string &b)
{
	b = std::string(j.get_string().c_str());
}
