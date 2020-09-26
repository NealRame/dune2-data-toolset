#pragma once

#include <ostream>
#include <iterator>
#include <string>
#include <vector>

namespace nr {
class PAK {
    struct impl;
    std::unique_ptr<impl> pimpl_;

public:
    struct Entry;

    class EntryBuffer : public std::streambuf {
        friend class PAK;
        struct impl;
        std::unique_ptr<impl> pimpl_;

    public:
        EntryBuffer(const Entry &);

    protected:
        virtual std::streambuf::int_type underflow() override;
    };

    struct Entry {
        const PAK &owner;
        std::string name;
        std::uintmax_t offset{0};
        std::uintmax_t size{0};
        EntryBuffer buffer() const;
    };

public:
    PAK();
    virtual ~PAK();

public:
    bool load(const std::string &filepath);

public:
    using const_iterator = std::vector<Entry>::const_iterator;
    using iterator = std::vector<Entry>::iterator;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;
};
} // namespace nr

namespace std {
    std::ostream &operator<<(std::ostream &, const nr::PAK::Entry &);
}
