#pragma once

#include <atomic>
#include <concepts>
#include <format>
#include <stdexcept>

template <std::size_t N>
requires (std::has_single_bit(N))
class SpscQueue {
public:
    using counter_type = std::size_t;
    using data_type = std::uint8_t;

    static constexpr std::size_t maxsize(void) { return N; }
    SpscQueue(void) : data_ptr_(new data_type[maxsize()])
        { }
    data_type const* data(void) const { return data_ptr_; }
    std::size_t physical_memory_location(std::size_t logical) {
        constexpr auto mask = N - 1;
        return logical & mask;
    }
// consumer only
    std::size_t size(void) const {
        auto wpos = producer_pos_.load(std::memory_order_acquire);
        auto rpos = consumer_pos_.load(std::memory_order_relaxed); // consumer owns this and does not need to sync with the producer
        return wpos - rpos;
    }
    bool pop(void) {
        auto wpos = producer_pos_.load(std::memory_order_acquire);
        auto rpos = consumer_pos_.load(std::memory_order_relaxed); // consumer owns this and does not need to sync with the producer
        if (!(wpos - rpos))
            return false; // empty
        consumer_pos_.store(rpos + 1, std::memory_order_release);
        return true;
    }
    bool pop(data_type& v) {
        auto wpos = producer_pos_.load(std::memory_order_acquire);
        auto rpos = consumer_pos_.load(std::memory_order_relaxed); // consumer owns this and does not need to sync with the producer
        if (!(wpos - rpos))
            return false; // empty
        v = data_ptr_[physical_memory_location(rpos)];
        consumer_pos_.store(rpos + 1, std::memory_order_release);
        return true;
    }
    void advance(std::size_t const num) {
        auto wpos = producer_pos_.load(std::memory_order_acquire);
        auto rpos = consumer_pos_.load(std::memory_order_relaxed); // No need to inform the producer since we're not changing this
        auto const left = wpos - rpos;
        if (num > left)
            throw std::out_of_range(std::format("attempted advance of {} with {} remaining", num, left));
        consumer_pos_.store(rpos + left, std::memory_order_release); // Tell consumer we've freed up space in the buffer
    }
private:
    bool empty(void) const { return !size(); }
    bool full(void) const { return size() >= maxsize(); }
// producer only
public:
    bool try_write();
private:
    // Bytes from producer position until physical memory boundary
    std::size_t from_physical_end_bound(void) const {
        auto const wpos = producer_pos_.load(std::memory_order_relaxed);
        return physical_memory_location(wpos);
    }
    std::size_t from_logical_end_bound(void) const {
        auto const rpos = consumer_pos_.load(std::memory_order_acquire);
        auto const wpos = producer_pos_.load(std::memory_order_relaxed); // owned by producer and not modified
        return maxsize() - wpos + rpos;
    }
    void full_write(std::size_t write_n) {
        // do you write stuff here
        producer_pos_.store(/* ... */write_n, std::memory_order_release);
    }
    void split_write(void); // when message > bytes until physical end bound
private:
    alignas(std::hardware_destructive_interference_size) std::atomic<counter_type> producer_pos_{0};
    alignas(std::hardware_destructive_interference_size) std::atomic<counter_type> consumer_pos_{0};
    data_type const* const data_ptr_;

};

int main(void) {
    SpscQueue<1024> q{};
}
