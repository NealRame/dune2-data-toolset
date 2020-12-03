#include "app.hpp"

AppState::Resource::Resource(const fs::path &filepath) 
    : filepath_{filepath} {
    if (fs::is_regular_file(filepath_)) {
        rc_.deserialize(filepath_);
    }
}

AppState::Resource::~Resource() {
    rc_.serialize(filepath_);
}

nr::dune2::Resource *
AppState::Resource::operator->() {
    return &rc_;
}

nr::dune2::Resource &
AppState::Resource::operator*() {
    return rc_;
}

const nr::dune2::Resource *
AppState::Resource::operator->() const {
    return &rc_;
}

const nr::dune2::Resource &
AppState::Resource::operator*() const {
    return rc_;
}

AppState::Resource AppState::resource() {
    return Resource(dune2RCPath);
}