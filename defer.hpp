#pragma once

/*
 *  Class for cleanup. Defer functions and this class will call
 *  them once each in a FILO manner.
 */
template <typename T = void(*)(void)>
class Defer {
public:
    Defer() = default;
    Defer(T fn) : defer_stack_{fn} { }
    // --- Move Constructors
    Defer(Defer const& other) = delete;
    Defer& operator=(Defer const& other) = delete;
    // --- Copy Constructors
    Defer& operator=(Defer&& other) noexcept {
        if (*this == other)
            return *this;
        defer_stack_ = std::move(other.defer_stack_);
        other.defer_stack_.clear(); // Make sure the moved away vectors don't cause a double defer call
        return *this;
    }
    Defer(Defer&& other) noexcept : defer_stack_{std::move(other.defer_stack_)} {
        other.defer_stack_.clear(); // Make sure the moved away vectors don't cause a double defer call
    }
    void defer(T fn) { defer_stack_.push_back(fn); }
    void undefer(void) { defer_stack_.pop_back(); }
    void clear(void) { defer_stack_ = {}; }
    ~Defer() {
        for (auto const& fn : defer_stack_ | std::views::reverse)
            fn();
    }
private:
    std::vector<T> defer_stack_{};
};