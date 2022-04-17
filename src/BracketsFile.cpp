#include "BracketsFile.h"

#include "re2/re2.h"

re2::RE2 expNumber("-?[\\d]+(\\.[\\d]+)?");
re2::RE2 expDateTime("[0-3][0-9]{3}[0-1][0-9][0-3][0-9][0-2][0-9]([0-5][0-9]){2}");
re2::RE2 expQuid("(?i:([a-f\\d]{8}(-[a-f\\d]{4}){3}-[a-f\\d]{12}))");

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
			if (text->at(position - 1) != '}') // конец массива
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

std::string BracketsFile::getStringInternal(BracketsFile* data) {

	if (data->isArray()) {
		std::string result;
		size_t count = 0;
		BracketsFile* item;
		for (size_t i = 0; i < data->size(); i++) {
			count++;
			result.push_back(',');
			result.append(getStringInternal(&data->getChildren(i)));
		}
		return "{\"#\",51e7a0d2-530b-11d4-b98a-008048da3034,{" + std::to_string(count) + result + "}}";
	}
	else
		return setType(data->get());
}

std::string BracketsFile::setType(std::string value, std::string type) {

	if (value.size() == 0)
		return "{\"U\"}"; // Неопределено

	std::string result(value);
	if (type.size() == 0) {
		if (RE2::FullMatch(value, expDateTime)) // дата время
			type = TYPE_DATETIME;
		else if (RE2::FullMatch(value, expNumber)) // Число
			type = TYPE_NUMBER;
		else if (RE2::FullMatch(value, expQuid)) // УникальныйИдентификатор
			type = TYPE_QUID;
		else if (value.front() == '"' && value.back() == '"') // Строка
			type = TYPE_STRING;
		else { // иначе строка
			type = TYPE_STRING;
			for (std::string::size_type pos{}; result.npos != (pos = result.find("\"", pos, 1)); pos += 2) // инкрементация кавычки
				result.replace(pos, 1, "\"\"", 2);
			result.insert(0, 1, '"');
			result.push_back('"');
		}
	}

	return "{" + type + "," + result + "}";
}