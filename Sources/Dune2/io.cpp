#include "io.hpp"

namespace nr::dune2::io {
std::string
readString(std::istream &in) {
    char c;
    std::string name;
    while (in && (c = in.get()) != '\0') {
        name.push_back(c);
    }
    return name;
}
} // namespace nr::dune2::io


std::ostream &
operator<<(
    std::ostream &out,
    const std::function<std::ostream &(std::ostream &)> &s) {
    return s(out);
}