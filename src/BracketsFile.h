#pragma once

#include <vector>
#include <string>

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
	std::string get() {
		return text->substr(begin, end - begin + 1);
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
	size_t isArray() {
		return text->at(begin) == '{' && text->at(end) == '}';
	}
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
};