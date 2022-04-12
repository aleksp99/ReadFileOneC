#include "BracketsFile.h"

void BracketsFile::loadArray() {
	isLoad = true;
	size_t lastPosition = begin;
	size_t position = begin;
	int count;
	while (nextToken(position, "\",{}")) {
		switch (text->at(position)) {
		case '{':
			lastPosition = position;
			count = 0;
			do {
				nextToken(position, "\"{}");
				if (text->at(position) == '{')
					count++;
				else
					count--;
			} while (count >= 0);
			childrens.push_back(BracketsFile(text, lastPosition, position, true));
			break;
		case ',':
			if (text->at(position - 1) != '}') // конец массива
				childrens.push_back(BracketsFile(text, lastPosition, position, false));
			break;
		case '}':
			childrens.push_back(BracketsFile(text, lastPosition, position, false));
			return;
		default:
			throw std::exception();
		}
		lastPosition = position;
	}
}
bool BracketsFile::nextToken(size_t& position, std::string tokens) {
	position++;
	size_t count = 0;
	for (std::string::size_type pos = position; text->npos != (pos = text->find_first_of(tokens, pos)); pos++) {
		if (text->at(pos) == '"')
			count++;
		else if (count % 2 == 0) {
			position = pos;
			return true;
		}
	}
	return false;
}
