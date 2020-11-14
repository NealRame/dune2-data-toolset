#pragma once

#include <iterator>
#include <optional>
#include <fstream>
#include <string>
#include <vector>

namespace nr::dune2 {
class PAK {
public:
    struct Entry {
        const size_t offset{0};
        const size_t size{0};
        const std::string name;
        const std::shared_ptr<std::string> filepath;

        std::string read() const;
    };

public:
    static std::optional<PAK> load(const std::string &filepath);

public:
    using const_iterator = std::vector<Entry>::const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    std::vector<Entry> entries_;
};
} // namespace nr::dune2
