#include <interface/interface.h>

int main(int count, char **args) {
    return Interface(count, args).exec();
}
