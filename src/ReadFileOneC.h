#ifndef READFILEONEC_H
#define READFILEONEC_H

#include "Component.h"

#include "BracketsFile.h"

class ReadFileOneC final : public Component {
public:
    const char *Version = u8"1.0.0";

    ReadFileOneC();

private:
    std::string extensionName() override;

    std::string text;
    BracketsFile data;

    // api interface
    variant_t open(variant_t path, variant_t mode);
    variant_t getText(variant_t toArray);
    variant_t getValue(variant_t indexs);
    variant_t regSearch(variant_t pattern);

    // utility methods
    void unpack(std::string& path);
    void read(std::string& path);
    void decode(std::string& path);

    std::string toStringInternal();
    std::string toStringInternal(size_t& position);
    void addValueToResult(std::string& result, std::string& value);
    void addValueToResult(std::string& result, std::string& value, size_t& countValue);

};

#endif //READFILEONEC_H
