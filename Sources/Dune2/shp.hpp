#pragma once

#include <Dune2/palette.hpp>
#include <Dune2/surface.hpp>

#include <filesystem>
#include <optional>
#include <vector>
#include <memory>

namespace nr::dune2 {
class SHP {
public:
    enum Version {
        v100 = 100,
        v107 = 107,
    };

    class Frame : public Surface {
    public:
        Frame(std::istream &, std::istream::pos_type);
        Frame(Frame &&);
        Frame &operator=(Frame &&);

        virtual ~Frame();

    public:
        virtual std::size_t getWidth() const override;
        virtual std::size_t getHeight() const override;
        virtual std::size_t getPixel(std::size_t, std::size_t) const override;
    
    private:
        struct impl;
        std::unique_ptr<impl> d;
    };

public:
    static std::optional<SHP> load(const std::string &filepath);

public:
    using const_iterator = std::vector<Frame>::const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    std::vector<Frame> frames_;
};

} // namespace nr::dune2
