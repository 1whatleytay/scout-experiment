#include <util/util.h>

#include <fstream>

std::vector<u8> loadFile(const std::string &path) {
    std::ifstream stream(path, std::ios::ate | std::ios::binary);

    assert(stream.is_open());

    std::vector<u8> data(stream.tellg());
    stream.seekg(0, std::ios::beg);
    stream.read(reinterpret_cast<char *>(data.data()), data.size());

    stream.close();

    return data;
}

void writeFile(const std::string &path, const std::vector<u8> &data) {
    std::ofstream stream(path, std::ios::trunc | std::ios::binary);

    assert(stream.is_open());

    stream.write(reinterpret_cast<const char *>(data.data()), data.size());

    stream.close();
}

Scanner::Scanner(const u8 *data) : data(data) { }
