#include <rom/rom.h>

//Header::Header(const u8 *data) {
//    Scanner scanner(data);
//
//    magic = swap(scanner.pop<u32>());
//    clockRate = swap(scanner.pop<u32>());
//    pc = swap(scanner.pop<u32>());
//    releaseAddress = swap(scanner.pop<u32>());
//    checksum = swap(scanner.pop<u64>());
//    unknown1 = swap(scanner.pop<u64>());
//    scanner.pop(name, sizeof(name));
//    unknown2 = swap(scanner.pop<u32>());
//    format = swap(scanner.pop<u32>());
//    scanner.pop(cartrigeId, sizeof(cartrigeId));
//    countryCode = scanner.pop<char>();
//    version = scanner.pop<u8>();
//    scanner.pop(bootstrap, sizeof(bootstrap));
//}

Rom::Rom(std::vector<u8> data) : data(std::move(data)) {
    header = *reinterpret_cast<Header *>(this->data.data());
}