#include <string>
#include <stdexcept>

// string view for c++14
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view
{
public:
    using traits_type = Traits;
    using value_type = CharT;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;

    using const_iterator = const value_type *;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;

    static constexpr size_type npos = size_type(-1);

public:
    // Constructors
    constexpr basic_string_view() noexcept : str_(nullptr), len_(0) {}
    constexpr basic_string_view(const basic_string_view &) noexcept = default;
    constexpr basic_string_view(const CharT *s, size_type count) : str_(s), len_(count) {}
    constexpr basic_string_view(const CharT *s) : str_(s), len_(traits_type::length(s)) {}

    // Assignment
    constexpr basic_string_view &operator=(const basic_string_view &) noexcept = default;

    // Iterators
    constexpr const_iterator begin() const noexcept { return this->str_; }
    constexpr const_iterator cbegin() const noexcept { return this->str_; };

    constexpr const_iterator end() const noexcept { return this->str_ + this->len_; }
    constexpr const_iterator cend() const noexcept { return this->str_ + this->len_; }

    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(this->end()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(this->end()); }

    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(this->begin()); }
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(this->begin()); }

    // Capacity
    constexpr size_type size() const noexcept { return this->len_; }
    constexpr size_type length() const noexcept { return this->len_; }

    constexpr size_type max_size() const noexcept;

    constexpr bool empty() const noexcept { return this->len_ == 0; }

    // Element access
    constexpr const_reference operator[](size_type pos) const
    {
        return *(this->str_ + pos);
    }

    constexpr const_reference at(size_type pos) const
    {
        if (pos >= this->len_)
        {
            throw std::out_of_range("basic_string_view::at out of range");
        }
        return *(this->str_ + pos);
    }

    constexpr const_reference front() const { return *this->str_; }
    constexpr const_reference back() const { return *(this->str_ + this->len_ - 1); }
    constexpr const_pointer data() const noexcept { return this->str_; }

    // Modifiers
    constexpr void remove_prefix(size_type n)
    {
        this->str_ += n;
        this->len_ -= n;
    }

    constexpr void remove_suffix(size_type n)
    {
        this->len_ -= n;
    }

    constexpr void swap(basic_string_view &v) noexcept
    {
        auto temp = *this;
        *this = v;
        v = temp;
    }

    constexpr basic_string_view substr(size_type pos = 0,
                                       size_type count = npos) const
    {
        if (pos > this->len_)
        {
            throw std::out_of_range{"basic_string_view::substr out of range"};
        }
        const size_type n = std::min(count, this->len_ - count);
        return basic_string_view(this->str_ + pos, n);
    }

private:
    const CharT *str_;
    size_type len_;
};

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const basic_string_view<CharT, Traits> &v)
{
    os.write(v.data(), static_cast<std::streamsize>(v.size()));
    return os;
}

using string_view = basic_string_view<char>;
