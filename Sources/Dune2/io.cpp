#include "io.hpp"

#include <sstream>

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
readAll(std::istream &in) {
    std::ostringstream oss;
    std::copy(
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>(),
        std::ostreambuf_iterator<char>(oss)
    );
    return oss.str();
}

std::string
readString(std::istream &in) {
    std::string s;
    std::getline(in, s, '\0');
    return s;
}

std::string
readString(std::istream &in, size_t size) {
    std::string s(size, '\0');
    in.read(std::data(s), size);
    return s;
}
} // namespace nr::dune2::io

std::ostream &
operator<<(
    std::ostream &out,
    const std::function<std::ostream &(std::ostream &)> &s) {
    return s(out);
}
