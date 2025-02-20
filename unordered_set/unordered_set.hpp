#ifndef UNORDERED_SET_UNORDERED_SET_HPP
#define UNORDERED_SET_UNORDERED_SET_HPP

#include <forward_list>
#include <vector>
#include <utility>

#define ITERATOR_IMPLEMENTED
#define FORWARD_LIST_IMPLEMENTED

template <class KeyT, class HashT = std::hash<KeyT>, class EqualT = std::equal_to<KeyT>>
class UnorderedSet {
  using ListType = std::forward_list<KeyT>;

 public:
  // member types

  using KeyType = KeyT;
  using ValueType = KeyT;
  using SizeType = std::size_t;
  using DifferenceType = std::ptrdiff_t;
  using Hasher = HashT;
  using KeyEqual = EqualT;
  using Reference = ValueType&;
  using ConstReference = const ValueType&;
  using Pointer = ValueType*;
  using ConstPointer = const ValueType*;
  using Iterator = typename ListType::const_iterator;
  using ConstIterator = typename ListType::const_iterator;

 private:
  ListType elements_;
  std::vector<Iterator> buckets_;
  SizeType size_ = 0;

  static constexpr Hasher kHash{};     // considered stateless
  static constexpr KeyEqual kEqual{};  // considered stateless
  static constexpr SizeType kExpandFactor = 2;

  [[nodiscard]] static auto Hash(const KeyType& key) {
    return kHash(key);
  }

  [[nodiscard]] static bool Equal(const KeyType& lhs, const KeyType& rhs) {
    return kEqual(lhs, rhs);
  }

  [[nodiscard]] static bool IsNullIterator(const ConstIterator iterator) noexcept {
    return iterator == ConstIterator{};
  }

  [[nodiscard]] bool BucketEmpty(const SizeType bucket_id) const noexcept {
    return IsNullIterator(BucketBeforeBegin(bucket_id));
  }

  [[nodiscard]] ConstIterator BucketBeforeBegin(const SizeType bucket_id) const noexcept {
    return buckets_[bucket_id];
  }

  [[nodiscard]] Iterator BucketBeforeBegin(const SizeType bucket_id) noexcept {
    return buckets_[bucket_id];
  }

  void BucketUpdate(const SizeType bucket_id, Iterator before_begin) noexcept {
    buckets_[bucket_id] = std::move(before_begin);
  }

  [[nodiscard]] ConstIterator BucketFindBefore(const SizeType bucket_id, const KeyType& key) const noexcept {
    if (auto before_element = BucketBeforeBegin(bucket_id); !IsNullIterator(before_element)) {
      for (auto element = std::next(before_element); element != end() && Bucket(*element) == bucket_id; ++element) {
        if (Equal(*element, key)) {
          return before_element;
        }
        before_element = element;
      }
    }
    return end();
  }

  [[nodiscard]] Iterator BucketFindBefore(const SizeType bucket_id, const KeyType& key) noexcept {
    return Iterator{std::as_const(*this).BucketFindBefore(bucket_id, key)};
  }

  [[nodiscard]] ConstIterator FindBefore(const KeyT& key) const noexcept {
    return BucketCount() > 0 ? BucketFindBefore(Bucket(key), key) : ConstIterator{};
  }

  [[nodiscard]] Iterator FindBefore(const KeyT& key) noexcept {
    return BucketCount() > 0 ? BucketFindBefore(Bucket(key), key) : Iterator{};
  }

  [[nodiscard]] SizeType CalculateCapacity(const SizeType delta) const noexcept {
    return BucketCount() + std::max(BucketCount(), delta);
  }

  void Splice(ListType& source, ConstIterator before_element) {
    const auto element = std::next(before_element);
    const auto bucket_id = Bucket(*element);
    if (auto bucket_before_begin = BucketBeforeBegin(bucket_id); bucket_before_begin != end()) {
      elements_.splice_after(bucket_before_begin, source, before_element);
    } else {  // bucket is empty
      const auto begin_before_insert = begin();
      elements_.splice_after(elements_.before_begin(), source, before_element);
      BucketUpdate(bucket_id, elements_.before_begin());
      if (begin_before_insert != end()) {
        BucketUpdate(Bucket(*begin_before_insert), begin());
      }
    }
  }

  template <class InputIterator>
  UnorderedSet(InputIterator begin, InputIterator end, const SizeType bucket_count) : buckets_(bucket_count) {
    Insert(std::move(begin), std::move(end));
  }

 public:
  UnorderedSet() = default;

  explicit UnorderedSet(const SizeType bucket_count) : buckets_(bucket_count) {
  }

  template <class ForwardIterator>
  UnorderedSet(ForwardIterator begin, ForwardIterator end)
      : UnorderedSet(std::move(begin), std::move(end), std::distance(begin, end)) {
  }

  UnorderedSet(const UnorderedSet& other) : UnorderedSet(other.cbegin(), other.cend(), other.Size()) {
  }

  UnorderedSet(UnorderedSet&& other) noexcept : UnorderedSet() {
    Swap(other);
  }

  // assignment

  UnorderedSet& operator=(const UnorderedSet& other) {
    if (this != &other) {
      UnorderedSet(other).Swap(*this);
    }
    return *this;
  }

  UnorderedSet operator=(UnorderedSet&& other) noexcept {
    if (this != &other) {
      UnorderedSet(std::move(other)).Swap(*this);
    }
    return *this;
  }

  // iterators

  [[nodiscard]] Iterator begin() noexcept {  // NOLINT
    return elements_.begin();
  }

  [[nodiscard]] ConstIterator begin() const noexcept {  // NOLINT
    return elements_.begin();
  }

  [[nodiscard]] ConstIterator cbegin() const noexcept {  // NOLINT
    return begin();
  }

  [[nodiscard]] Iterator end() noexcept {  // NOLINT
    return elements_.end();
  }

  [[nodiscard]] ConstIterator end() const noexcept {  // NOLINT
    return elements_.end();
  }

  [[nodiscard]] ConstIterator cend() const noexcept {  // NOLINT
    return end();
  }

  // data access

  [[nodiscard]] SizeType Size() const noexcept {
    return size_;
  }

  [[nodiscard]] bool Empty() const noexcept {
    return Size() == 0;
  }

  void Clear() noexcept {
    elements_.clear();
    std::fill(buckets_.begin(), buckets_.end(), Iterator{});
    size_ = 0;
  }

  // modifiers

  template <class... Args>
  bool Emplace(Args&&... args) {
    auto single_node = ListType{};
    single_node.emplace_front(std::forward<Args>(args)...);
    if (Find(single_node.front())) {
      return false;
    }
    if (BucketCount() == Size()) {
      Rehash(CalculateCapacity(1u));
    }
    Splice(single_node, single_node.cbefore_begin());
    ++size_;
    return true;
  }

  bool Insert(const ValueType& value) {
    return Emplace(value);
  }

  bool Insert(ValueType&& value) {
    return Emplace(std::move(value));
  }

  template <class InputIterator>
  void Insert(InputIterator begin, InputIterator end) {
    for (; begin != end; ++begin) {
      Emplace(*begin);
    }
  }

  SizeType Erase(const KeyType& key) {
    const auto before_element = FindBefore(key);
    if (before_element == end()) {
      return 0;
    }

    const auto bucket_id = Bucket(key);
    const auto bucket_before_begin = BucketBeforeBegin(bucket_id);
    const auto after_element = std::next(before_element, 2);
    if (after_element == end()) {
      if (bucket_before_begin == before_element) {
        BucketUpdate(bucket_id, Iterator{});
      }
    } else if (const auto after_bucket_id = Bucket(*after_element); after_bucket_id != bucket_id) {
      BucketUpdate(after_bucket_id, before_element);
      if (bucket_before_begin == before_element) {
        BucketUpdate(bucket_id, Iterator{});
      }
    }
    elements_.erase_after(before_element);
    --size_;
    return 1;
  }

  void Swap(UnorderedSet& other) noexcept {
    elements_.swap(other.elements_);
    buckets_.swap(other.buckets_);
    std::swap(size_, other.size_);

    // swap invalidates before_begin
    if (!Empty()) {
      BucketUpdate(Bucket(*begin()), elements_.before_begin());
    }
    if (!other.Empty()) {
      other.BucketUpdate(other.Bucket(*other.begin()), other.elements_.before_begin());
    }
  }

  [[nodiscard]] bool Find(const KeyType& key) const {
    return FindBefore(key) != end();
  }

  // hashtable

  [[nodiscard]] SizeType BucketCount() const noexcept {
    return buckets_.size();
  }

  [[nodiscard]] SizeType BucketSize(const SizeType bucket_id) const {
    if (bucket_id >= BucketCount() || BucketEmpty(bucket_id)) {
      return 0;
    }
    auto size = SizeType{0};
    for (auto begin = std::next(BucketBeforeBegin(bucket_id)), end = this->end();
         begin != end && Bucket(*begin) == bucket_id; ++begin) {

      ++size;
    }
    return size;
  }

  [[nodiscard]] SizeType Bucket(const KeyType& key) const {
    const auto hash_value = Hash(key);
    const auto bucket_count = BucketCount();
    return hash_value % bucket_count;
  }

  [[nodiscard]] float LoadFactor() const {
    const auto bucket_count = BucketCount();
    const auto size = static_cast<float>(Size());
    return bucket_count ? size / bucket_count : 0;
  }

  void Rehash(const SizeType new_bucket_count) {
    if (new_bucket_count < Size() || new_bucket_count == BucketCount()) {
      return;
    }
    auto tmp_list = std::move(elements_);
    elements_.clear();  // ensure empty after moved-from state
    buckets_.assign(new_bucket_count, Iterator{});

    while (!tmp_list.empty()) {
      Splice(tmp_list, tmp_list.before_begin());
    }
  }

  void Reserve(const SizeType element_count) {
    if (element_count <= BucketCount()) {
      return;
    }
    Rehash(element_count);
  }
};

#endif  // UNORDERED_SET_UNORDERED_SET_HPP