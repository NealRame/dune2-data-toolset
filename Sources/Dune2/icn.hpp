#pragma once

#include <Dune2/palette.hpp>
#include <Dune2/surface.hpp>

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace nr::dune2 {
class ICN {
public:
    struct Info {
        unsigned int width;
        unsigned int height;
        unsigned int bitPerPixels;

        std::size_t tileSize() const
        { return (width*height*bitPerPixels)/8; }

        std::size_t paletteSize() const
        { return 1<<bitPerPixels; }
    };

    class Icon: public Surface {
        friend class ICN;

    public:
        Icon(Icon &&);
        Icon &operator=(Icon &&);
        virtual ~Icon();

    public:
        virtual std::size_t getWidth() const override;
        virtual std::size_t getHeight() const override;
        virtual Color getPixel(std::size_t x, std::size_t y) const override;

    private:
        Icon(const Palette &, size_t, size_t, const Info &);

    private:
        struct impl;
        std::unique_ptr<impl> d;
    };

public:
    static std::optional<ICN> load(
        const std::string &map_filepath,
        const std::string &icn_filepath,
        const Palette &
    );

public:
    using const_iterator = std::vector<Icon>::const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    std::vector<Icon> icons_;
};
} // namespace nr::dune2
