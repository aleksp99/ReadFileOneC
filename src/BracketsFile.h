#pragma once

#include <vector>
#include <string>

#define TYPE_DATETIME "\"D\""
#define TYPE_NUMBER "\"N\""
#define TYPE_QUID "\"#\",fc01b5df-97fe-449b-83d4-218a090e681e"
#define TYPE_STRING "\"S\""

class BracketsFile {
public:
	BracketsFile() {}
	BracketsFile(std::string& input) {
		begin = input.find('{');
		end = input.rfind('}');
		text = &input;
		loadArray();
	}
	size_t begin = 0;
	size_t end = 0;
	std::string* text = nullptr;
	std::string get(bool addType = false) {
		return addType ? getStringInternal(this) : text->substr(begin, end - begin + 1);
	}
	BracketsFile& getChildren(size_t index) {
		if (!isLoad)
			loadArray();
		return childrens[index];
	}
	bool empty() { return text == nullptr; }
	size_t size() {
		if (!isLoad)
			loadArray();
		return childrens.size();
	}
	bool isArray() {
		return text->at(begin) == '{' && text->at(end) == '}';
	}
	static std::string setType(std::string value, std::string type = "");
private:
	bool isLoad = false;
	std::vector<BracketsFile> childrens;
	BracketsFile(std::string* input, size_t& _begin, size_t _end, bool isArray) {
		begin = isArray ? _begin : _begin + 1;
		end = isArray ? _end : _end - 1;
		text = input;
		isLoad = !isArray;
	}
	void loadArray();
	bool nextToken(size_t& position, std::string tokens);
	static std::string getStringInternal(BracketsFile* data);
};