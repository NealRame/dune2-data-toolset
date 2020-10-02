#pragma once

#include <iterator>
#include <optional>
#include <fstream>
#include <string>
#include <vector>

namespace nr::dune2 {
class PAK {
public:
    struct Entry;

    class EntryBuffer : public std::streambuf {
    public:
        EntryBuffer(const Entry &);

    protected:
        virtual std::streambuf::int_type underflow() override;
    
    private:
        const std::size_t offset_{0};
        const std::size_t size_{0};
        std::size_t pos_;
        char ch_;
        std::fstream input_;
    };

    struct Entry {
        const std::size_t offset{0};
        const std::size_t size{0};
        const std::string name;
        const std::shared_ptr<std::string> filepath;

        EntryBuffer buffer() const;
    };

public:
    static std::optional<PAK> load(const std::string &filepath);

public:
    using const_iterator = std::vector<Entry>::const_iterator;
    using iterator = std::vector<Entry>::iterator;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    std::vector<Entry> entries_;
};
} // namespace nr::dune2

namespace std {
    std::ostream &operator<<(std::ostream &, const nr::dune2::PAK::Entry &);
}
