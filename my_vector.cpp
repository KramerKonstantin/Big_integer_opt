#include "my_vector.h"
#include <cstring>

my_vector::my_vector() : vector_size(0) {}

my_vector::my_vector(my_vector const &vector) : is_vector_created(vector.is_vector_created), vector_size(vector.vector_size){
    if (vector.is_vector_created) {
        new (&_data.big_data) std::shared_ptr<std::vector<uint32_t>>(vector._data.big_data);
    } else {
        for (size_t i = 0; i < SIZE; i++) {
            _data.small_data[i] = vector._data.small_data[i];
        }
    }
}

my_vector &my_vector::operator=(my_vector const &vector) {
    is_vector_created = vector.is_vector_created;
    vector_size = vector.vector_size;
    if (vector.is_vector_created) {
        new (&_data.big_data) std::shared_ptr<std::vector<uint32_t>>(vector._data.big_data);
    } else {
        for (size_t i = 0; i < SIZE; i++) {
            _data.small_data[i] = vector._data.small_data[i];
        }
    }
    return *this;
}

my_vector::my_vector(size_t size) : vector_size(size) {
    if (vector_size > SIZE) {
        is_vector_created = true;
        new (&_data.big_data) std::shared_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>(size));
    }
}

my_vector::my_vector(size_t size, uint32_t value) : vector_size(size) {
    if (vector_size > SIZE) {
        is_vector_created = true;
        new (&_data.big_data) std::shared_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>(size, value));
    } else {
        for (unsigned int &i : _data.small_data) {
            i = value;
        }
    }
}

my_vector::~my_vector() {
    if (is_vector_created) {
        _data.big_data.reset();
    }
}

size_t my_vector::size() const {
    return vector_size;
}

bool my_vector::empty() const {
    return vector_size == 0;
}

void my_vector::ensure_unique() {
    if (!_data.big_data.unique()) {
        auto ptr = new std::vector<uint32_t>(*(_data.big_data));
        _data.big_data.reset(ptr);
    }
}

void my_vector::push_back(uint32_t value) {
  if (!is_vector_created && vector_size < SIZE) {
      _data.small_data[vector_size] = value;
  } else {
      if (!is_vector_created) {
          to_big();
      }
      ensure_unique();
      _data.big_data->push_back(value);
  }
  vector_size++;
}

uint32_t my_vector::pop_back() {
    assert(vector_size != 0);
    if (!is_vector_created) {
        return _data.small_data[--vector_size];
    } else {
        ensure_unique();
        uint32_t t = _data.big_data->back();
        _data.big_data->pop_back();
        vector_size--;
        return t;
    }
}

void my_vector::push_front(uint32_t value) {
    if (!is_vector_created && vector_size < SIZE) {
        for (size_t i = vector_size; i > 0; i--) {
            _data.small_data[i] = _data.small_data[i - 1];
        }
        _data.small_data[0] = value;
    } else {
        if (!is_vector_created) {
            to_big();
        }
        ensure_unique();
        auto it = _data.big_data->begin();
        _data.big_data->insert(it,value);
    }
    vector_size++;
}

uint32_t &my_vector::operator[](size_t index) {
    assert(index < vector_size);
    if (is_vector_created) {
        ensure_unique();
        return _data.big_data->at(index);
    } else {
        return _data.small_data[index];
    }
}

const uint32_t &my_vector::operator[](size_t index) const {
    assert(index < vector_size);
    if (is_vector_created) {
        return _data.big_data->at(index);
    } else {
        return _data.small_data[index];
    }
}

uint32_t &my_vector::back() {
    assert(vector_size > 0);
    if (is_vector_created) {
        ensure_unique();
        return _data.big_data->back();
    } else {
        return _data.small_data[vector_size - 1];
    }
}

void my_vector::swap_big_small(my_vector::any_data &big, my_vector::any_data &small) {
    uint32_t temp[SIZE];
    for (size_t i = 0; i < SIZE; i++) {
        temp[i] = small.small_data[i];
    }
    new(&small.big_data) std::shared_ptr<std::vector<uint32_t>>(big.big_data);
    big.big_data.reset();
    for (size_t i = 0; i < SIZE; i++) {
        big.small_data[i] = temp[i];
    }
}

void my_vector::swap(my_vector &a, my_vector &b) {
    if (!a.is_vector_created && !b.is_vector_created) {
        for (size_t i = 0; i < SIZE; i++) {
            std::swap(a._data.small_data[i], b._data.small_data[i]);
        }
    } else if (a.is_vector_created && b.is_vector_created) {
        std::swap(a._data.big_data, b._data.big_data);
    } else if (a.is_vector_created) {
        swap_big_small(a._data, b._data);
    } else {
        swap_big_small(b._data, a._data);
    }
    std::swap(a.is_vector_created, b.is_vector_created);
    std::swap(a.vector_size, b.vector_size);
}

bool operator==(my_vector const &a, my_vector const &b) {
    if (a.vector_size != b.vector_size) {
        return false;
    } else {
        for (size_t i = 0; i < a.vector_size; i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
    }
    return true;
}

void my_vector::resize(size_t size) {
    if (is_vector_created && size <= SIZE) {
        ensure_unique();
        _data.big_data->resize(size);
    }
    if (size > SIZE) {
        if (!is_vector_created) {
            to_big();
        }
        ensure_unique();
        _data.big_data->resize(size);
    }
    vector_size = size;
}

void my_vector::to_big() {
    is_vector_created = true;
    uint32_t temp[SIZE];
    for (size_t i = 0; i < SIZE; i++) {
        temp[i] = _data.small_data[i];
    }
    new (&_data.big_data) std::shared_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>());
    for (size_t i = 0; i < vector_size; i++) {
        _data.big_data->push_back(temp[i]);
    }
}
