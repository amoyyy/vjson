#pragma once
#include "json_parser/parser.h"
#include "json_encoder/encoder.h"
#include "json_utils/interface.h"

class Jdoc
{
public:
	Value *doc;

public:
	bool has_member(const FRString &key)
	{
		return doc->has_member(key);
	}

	// reader
	Value &operator[](const FRString &key)
	{
		return (*doc)[key];
	}

public:
	// writer
	void save_doc(const std::string &output_path)
	{
		Encoder ed(doc);
		std::ofstream file(output_path);
		file << ed.buffer.rdbuf();
		file.close();
	}

	// get Value from file
	Jdoc(const std::string &input_path)
	{
		doc = Parser(input_path).decode();
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