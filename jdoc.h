#pragma once
#include "json_parser/parser.h"
#include "json_encoder/encoder.h"

class Jdoc
{
	using json = utils::json::Value;
	using parser = utils::json::Parser;
	using encoder = utils::json::Encoder;

public:
	json *doc;

public:
	bool has_member(const utils::FRString &key)
	{
		return doc->has_member(key);
	}

	// reader
	json &operator[](const utils::FRString &key)
	{
		return (*doc)[key];
	}

public:
	// writer
	void save_doc(const std::string &output_path)
	{
		encoder ed(doc);
		std::ofstream file(output_path);
		file << ed.buffer.rdbuf();
		file.close();
	}

	// get Value from file
	Jdoc(const std::string &input_path)
	{
		doc = parser(input_path).decode();
	}

public:
	Jdoc() = default;
	Jdoc(const Jdoc &t) = delete;
	Jdoc(Jdoc &&t) = delete;
	Jdoc &operator=(const Jdoc &t) = delete;
	Jdoc &operator=(Jdoc &&t) = delete;

public:
	~Jdoc()
	{
		if (doc != nullptr)
			delete doc;
	}
};