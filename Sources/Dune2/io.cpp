#include "io.hpp"

namespace nr::dune2::io {

IPosOffsetGuard::IPosOffsetGuard(std::istream &input)
    : input_{input}
    , pos_{input_.tellg()} {
}

IPosOffsetGuard::~IPosOffsetGuard() {
    input_.seekg(pos_);
}

OPosOffsetGuard::OPosOffsetGuard(std::ostream &output)
    : output_{output}
    , pos_{output_.tellp()} {
}

OPosOffsetGuard::~OPosOffsetGuard() {
    output_.seekp(pos_);
}

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