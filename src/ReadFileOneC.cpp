#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <regex>

#include "ReadFileOneC.h"

#include "zlib.h"
#include "re2/re2.h"

#define CHUNK_SIZE 262144

// ToStringInternal
const std::string beginArray("{\"#\",51e7a0d2-530b-11d4-b98a-008048da3034,\n{");
std::regex expNumber("-?[\\d]+");
std::regex expDateTime("[0-3][0-9]{3}[0-1][0-9][0-3][0-9][0-2][0-9]([0-5][0-9]){2}");
std::regex expQuid("[a-f\\d]{8}(-[a-f\\d]{4}){3}-[a-f\\d]{12}", std::regex_constants::icase);

ReadFileOneC::ReadFileOneC() {
    
    // Full featured property registration example
    AddProperty(L"Version", L"ВерсияКомпоненты", [&]() { return std::make_shared<variant_t>(std::move(std::string(Version))); });

    AddMethod(L"Open", L"Открыть", this, &ReadFileOneC::open);
    AddMethod(L"Text", L"Текст", this, &ReadFileOneC::getText, { {0, false} });
    AddMethod(L"Value", L"Значение", this, &ReadFileOneC::getValue, { {1, false} });
    AddMethod(L"Find", L"Найти", this, &ReadFileOneC::regSearch);

}

std::string ReadFileOneC::extensionName() {
    return "ReadFileOneC";
}

// api interface
variant_t ReadFileOneC::open(variant_t path, variant_t mode) {
    text = "";
    data = BracketsFile();
    try {
        std::string fullName(std::get<std::string>(path));
        switch (std::get<int>(mode))
        {
        case 1:
            unpack(fullName);
            break;
        case 2:
            read(fullName);
            break;
        case 3:
            decode(fullName);
            break;
        default:
            break;
        }
        if (text.size() == 0) return false;
        //text.erase(0, 3); // до первого '{'
    }
    catch (...) {
        return false;
    }
    return true;
}
variant_t ReadFileOneC::getText(variant_t toStringInternal) {
    if (data.empty() && text.size() > 0)
        data = BracketsFile(text);
    return data.get(std::get<bool>(toStringInternal));
}
variant_t ReadFileOneC::getValue(variant_t indexs, variant_t toStringInternal) {

    if (data.empty() && text.size() > 0)
        data = BracketsFile(text);
    
    BracketsFile* cursor = &data;
    std::stringstream indexsStream(std::get<std::string>(indexs));
    std::string item;
    size_t index;
    while (std::getline(indexsStream, item, ',')) {
        index = std::stoi(item);
        if (cursor->isArray() && cursor->size() > index)
            cursor = &cursor->getChildren(index);
        else
            return std::monostate();
    }
    return cursor->get(std::get<bool>(toStringInternal));
}
variant_t ReadFileOneC::regSearch(variant_t pattern) {

    std::string result;
    size_t count(0);

    re2::RE2::Options options;
    options.set_log_errors(false);
    re2::RE2 re2(std::get<std::string>(pattern), options);

    re2::StringPiece::iterator text_it = re2::StringPiece(text).begin();
    re2::StringPiece value;
    while (RE2::PartialMatch(text_it, re2, &value)) {
        count++;
        std::string str(value.ToString());
        for (std::string::size_type pos{}; str.npos != (pos = str.find("\"", pos, 1)); pos += 2) // инкрементация кавычки
            str.replace(pos, 1, "\"\"", 2);
        result.push_back(',');
        result.append(BracketsFile::setType(str));
        text_it = value.begin() + value.size();
    }
    return "{\"#\",51e7a0d2-530b-11d4-b98a-008048da3034,{" + std::to_string(count) + result + "}}";
}

// utility methods
void ReadFileOneC::unpack(std::string& path)
{
    // https://github.com/e8tools/tool1cd

    int result;
    char inbuff[CHUNK_SIZE];
    uint8_t outbuff[CHUNK_SIZE];
    z_stream stream = { 0 };

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;
    result = inflateInit2(&stream, -MAX_WBITS);

    if (result != Z_OK)
        throw "inflateInit(...) failed!";

    std::ifstream fs(path, std::ios::binary);
    while (fs.good())
    {
        fs.read(inbuff, CHUNK_SIZE - 1);
        stream.next_in = (Bytef*)inbuff;
        stream.avail_in = fs.gcount();

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = outbuff;
            result = inflate(&stream, Z_NO_FLUSH);
            switch (result) {
            case Z_STREAM_ERROR:
                (void)inflateEnd(&stream);
                throw "The stream structure was inconsistent";
                break;
            case Z_NEED_DICT:
                result = Z_DATA_ERROR;
            case Z_DATA_ERROR:
                (void)inflateEnd(&stream);
                throw "Input data was corrupted";
                break;
            case Z_MEM_ERROR:
                (void)inflateEnd(&stream);
                throw "Not enough memory";
                break;
            }

            uint32_t nbytes = CHUNK_SIZE - stream.avail_out;
            for (size_t i = (text.size() == 0 ? 3 : 0); i < nbytes; i++)
            {
                if (outbuff[i] != '\r')
                    text.push_back(outbuff[i]);
            }

        } while (stream.avail_out == 0);
    }
    fs.good();
    inflateEnd(&stream);
}
void ReadFileOneC::read(std::string& path)
{
    std::ifstream fs(path);
    if (fs.good())
    {
        std::stringstream file_stream;
        file_stream << fs.rdbuf(); // reading data
        text = file_stream.str();
    }
    fs.close();
}
void ReadFileOneC::decode(std::string& path)
{
    // https://github.com/Infactum/onec_dtools

    std::ifstream fs(path, std::ios::binary);
    if (fs.good())
    {
        fs.seekg(0, std::ios::end);
        size_t size = fs.tellg();
        fs.seekg(0, std::ios::beg);

        char* buffer = new char[size]; // для больших файлов (сотни метров и больше) - ужасно
        fs.read(buffer, size);

        // Первый байт содержит длину маски шифрования
        // Далее каждая порция байт соответствующей длины поксорена на маску
        uint8_t max_length = buffer[0];
        size_t j = 1;
        for (size_t i = max_length + 1; i < size; i++)
        {
            text.push_back((buffer[j] ^ buffer[i]));
            j++;
            if (j > max_length)
                j = 1;
        }

        // освобождаем буфер
        delete[] buffer;
    }
    fs.close();
}