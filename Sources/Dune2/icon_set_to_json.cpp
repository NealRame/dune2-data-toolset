#include "icon_set.hpp"

namespace nr::dune2 {

using rapidjson::Document;
using rapidjson::Value;
using rapidjson::MemoryPoolAllocator;

namespace {
Value
icon_to_JSON(
    MemoryPoolAllocator<> &allocator,
    const IconSet::Icon &icon
) {
    Value images(rapidjson::kArrayType);
    for (auto &&image_index : icon.getTileIndexList()) {
        images.PushBack(Value((unsigned int) image_index), allocator);
    }

    Value shape(rapidjson::kObjectType);
    shape.AddMember("width", Value((unsigned int)icon.getColumnCount()), allocator);
    shape.AddMember("height", Value((unsigned int)icon.getRowCount()), allocator);

    Value value(rapidjson::kObjectType);
    value.AddMember("size", shape, allocator);
    value.AddMember("indexes", images, allocator);

    return value;
}

}

Document
IconSet::toJSON() const {
    Document doc;

    auto &allocator = doc.GetAllocator();
    auto &icons = doc.SetArray();
    
    for (auto &&icon : icons_) {
        icons.PushBack(icon_to_JSON(allocator, icon), allocator);
    }

    return doc;
}

} // namespace nr::dune2
