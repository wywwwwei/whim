#include <atomic>

class SharedCount
{
public:
    SharedCount() : count_(1) {}

    void add() { ++count_; }
    void minus() { --count_; }
    int get() const { return count_; }

private:
    std::atomic<int> count_;
};

template <typename T>
class SharedPtr
{
private:
    void clean()
    {
        if (ref_count_)
        {
            ref_count_->minus();
            if (ref_count_->get() == 0)
            {
                if (ptr_)
                    delete ptr_;
                delete ref_count_;
            }
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U> &p, T *ptr)
    {
        this->ptr_ = ptr;
        this->ref_count_ = p.ref_count_;
        ref_count_->add();
    }

public:
    SharedPtr() : ptr_(nullptr), ref_count_(new SharedCount) {}
    SharedPtr(T *ptr) : ptr_(ptr), ref_count_(new SharedCount) {}
    SharedPtr(const SharedPtr &p)
    {
        this->ptr_ = p.ptr_;
        this->ref_count_ = p.ref_count_;
        ref_count_->add();
    }
    SharedPtr(SharedPtr &&p)
    {
        this->ptr_ = p.ptr_;
        this->ref_count_ = p.ref_count_;
        p.ptr_ = nullptr;
        p.ref_count_ = nullptr;
    }
    SharedPtr &operator=(const SharedPtr &p)
    {
        clean();
        this->ptr_ = p.ptr_;
        this->ref_count_ = p.ref_count_;
        ref_count_->add();
        return *this;
    }
    SharedPtr &operator=(SharedPtr &&p)
    {
        clean();
        this->ptr_ = p.ptr_;
        this->ref_count_ = p.ref_count_;
        p.ptr_ = nullptr;
        p.ref_count_ = nullptr;
        return *this;
    }

    ~SharedPtr() { clean(); }

    int use_count() const { return ref_count_->get(); }
    T *get() const { return ptr_; }
    T *operator->() { return ptr_; }
    T &operator*() { return *ptr_; }
    operator bool() const { return ptr_; }

    template <typename U>
    friend class SharedPtr;

    template <typename K, typename U>
    friend SharedPtr<K> static_pointer_cast(const SharedPtr<U> &p);
    template <typename K, typename U>
    friend SharedPtr<K> dynamic_pointer_cast(const SharedPtr<U> &p);
    template <typename K, typename U>
    friend SharedPtr<K> const_pointer_cast(const SharedPtr<U> &p);
    template <typename K, typename U>
    friend SharedPtr<K> reinterpret_pointer_cast(const SharedPtr<U> &p);

private:
    T *ptr_;
    SharedCount *ref_count_;
};

template <typename T, typename U>
SharedPtr<T> static_pointer_cast(const SharedPtr<U> &p)
{
    T *ptr = static_cast<T *>(p.get());
    return SharedPtr(p, ptr);
}

template <typename T, typename U>
SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &p)
{
    if (auto ptr = dynamic_cast<T *>(p.get()))
    {
        return SharedPtr{p, ptr};
    }
    else
    {
        return SharedPtr<T>{};
    }
}

template <typename T, typename U>
SharedPtr<T> const_pointer_cast(const SharedPtr<U> &p)
{
    T *ptr = const_cast<T *>(p.get());
    return SharedPtr(p, ptr);
}

template <typename T, typename U>
SharedPtr<T> reinterpret_pointer_cast(const SharedPtr<U> &p)
{
    T *ptr = reinterpret_cast<T *>(p.get());
    return SharedPtr(p, ptr);
}

template <typename T>
class UniquePtr
{
private:
    void clean()
    {
        if (ptr_)
            delete ptr_;
    }

public:
    UniquePtr() : ptr_(nullptr) {}
    UniquePtr(T *ptr) : ptr_(ptr) {}

    UniquePtr(const UniquePtr &p) = delete;
    UniquePtr &operator=(const UniquePtr &p) = delete;

    UniquePtr(UniquePtr &&p)
    {
        this->ptr_ = p.ptr_;
        p.ptr_ = nullptr;
    }

    UniquePtr &operator=(UniquePtr &&p)
    {
        this->ptr_ = p.ptr_;
        p.ptr_ = nullptr;
        return *this;
    }

    ~UniquePtr() { clean(); }

    T *get() const { return ptr_; }
    T *operator->() const { return ptr_; }
    T &operator*() const { return *ptr_; }
    operator bool() const { return ptr_; }

private:
    T *ptr_;
};