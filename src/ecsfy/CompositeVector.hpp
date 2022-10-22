#include <vector>
#include <unordered_map>

template<typename _ComponentRegistry>
class CompositeVector {
 private:
  using ComponentRegistryType = _ComponentRegistry;
  using CompositionType = typename ComponentRegistryType::CompositionType;

 public:
  class Iterator {
   public:
    Iterator(char *location, size_t stride)
        : location(location), stride(stride) {}

    bool operator==(const Iterator &rhs) const {
      return location == rhs.location;
    }

    bool operator!=(const Iterator &rhs) const { return *this != rhs; }

    Iterator operator++() {
      auto copy = *this;
      ++*this;
      return copy;
    }
    Iterator &operator++(int) {
      location += stride;
      return *this;
    }

    Iterator operator--() {
      auto copy = *this;
      --*this;
      return copy;
    }

    Iterator &operator--(int) {
      location -= stride;
      return *this;
    }

    char *operator*() { return location; }
    const char *operator*() const { return location; }

   private:
    char *location;
    size_t stride;
  };

 public:
  CompositeVector(const int compositeSize) : compositeSize(compositeSize) {}

  void PushBack() {
    buffer.reserve(buffer.size() + compositeSize);
  }

  void Remove(std::size_t index) {
    // TODO
  }

  Iterator GetBegin() {
    return buffer.empty() ? GetEnd() : Iterator{buffer.data(), compositeSize};
  }

  Iterator GetEnd() {
    return {buffer.empty() ? nullptr : buffer.data() + buffer.size(),
            compositeSize};
  }

  template <typename Component>
  std::size_t CalcOffset() const {
    size_t offset = 0;
    for (std::size_t i = 0; i < ComponentRegistryType::template GetIndex<Component>(); ++i) {
      if (composition.test(i))
        offset += ComponentRegistryType::GetSize(i);
    }
    return offset;
  }

 private:
  CompositionType composition;
  std::vector<char> buffer;
  const int compositeSize;
};
