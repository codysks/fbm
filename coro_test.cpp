#include <fstream>
#include <stdexcept>
#include <format>
#include <utility>
#include <string_view>
#include <atomic>
#include <coroutine>
#include <iostream>
#include <optional>
#include <vector>
#include <cassert>
#include <cctype> 


#include "char_enum.hpp"


struct SbtcpMessage {
    inline SbtcpMessage& copy(std::vector<unsigned char>&& other) {
        if (&(this->data_) != &other) {
            data_ = std::move(other);
        }
        return *this;
    }
    SoupbinTcpMessageType message_type_;
    std::vector<unsigned char> data_;
};

template <typename T>
struct Generator {
    struct promise_type {
        auto get_return_object(void) noexcept { return Generator{ *this }; }
        std::suspend_always initial_suspend(void) const noexcept { return {}; }
        std::suspend_always final_suspend(void) const noexcept { return {}; }
        std::suspend_always yield_value(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
            result = value;
            return {};
        }
        void return_void(void) const noexcept {}
        void unhandled_exception(void) noexcept(std::is_nothrow_copy_constructible_v<std::exception_ptr>) {
            result = std::current_exception();
        }
        T& getValue(void) {
            if (std::holds_alternative<std::exception_ptr>(result)) {
                std::rethrow_exception(std::get<std::exception_ptr>(result));
            }
            return std::get<T>(result);
        }
    private:
        std::variant<std::monostate, T, std::exception_ptr> result;
    };
    Generator(Generator&& other) noexcept : coro{ std::exchange(other.coro, nullptr) } {}
    Generator& operator=(Generator&& other) noexcept {
        if (coro)
            coro.destroy();
        coro = std::exchange(other.coro, nullptr);
    }
    ~Generator(void) {
        if (coro)
            coro.destroy();
    }
    auto& operator()() const {
        coro(); // same as coro.resume()
        return coro.promise().getValue();
    }
private:
    explicit Generator(promise_type& promise) noexcept :
        coro{ std::coroutine_handle<promise_type>::from_promise(promise) }
    {}

    std::coroutine_handle<promise_type> coro;
};


Generator<std::optional<SbtcpMessage>> read_file(std::string const& path) {
    SbtcpMessage data_container;
    size_t message_start_byte = 0;
    size_t total_bytes_read = 0;
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open())
        throw std::runtime_error(std::format("{}(): Could not open file {}", __func__, path));
    auto read_one_char = [&](unsigned char& c) {
        int nextchar = ifs.get();
        if (ifs.eof()) {
            ifs.clear();
            return false;
        }
        ++total_bytes_read;
        c = (unsigned char)nextchar;
        return true;
    };

start_cycle:
    message_start_byte = total_bytes_read;
// Read Sbtcp length packet
    unsigned char len_str[2];
    for (size_t i = 0; i < sizeof(len_str); ++i) {
        while (true) {
            if (!read_one_char(len_str[i]))
                co_yield {}; // co_yield is equivalent to co_await promise.yield_value(expression);
            else
                break;
        }
    }
    unsigned len = len_str[0] * 256 + len_str[1];

// Read Sbtcp message type
    unsigned char msgtype;
    while (true) {
        if (!read_one_char(msgtype))
            co_yield {};
        else
            break;
    }
    data_container.message_type_.unpack(msgtype);
    assert(len >= 1);
    --len; // Remove Sbtcp message type char
    std::cerr << std::format("DEBUG: {{ start={}, length={}, msgtype={} }}\n", message_start_byte, len + sizeof(len_str), data_container.message_type_);

// Read the rest of the message
    std::vector<unsigned char> packet(len);

    for (size_t i = 0; i < len; ++i) {
        while (true) {
            if (!read_one_char(packet[i]))
                co_yield {};
            else
                break;
        }
    }
    // std::string s{};
    // s += std::format("{:02x}|{:02x}|{:02x}|", (unsigned)len_str[0], (unsigned)len_str[1], (unsigned)msgtype);
    // for (auto const& b : packet) {
    //     s += std::format("{:02x}|", (unsigned)b);
    // }
    // s.pop_back();
    // std::cerr << s << std::endl;
    data_container.copy(std::move(packet));
    co_yield data_container;

    goto start_cycle;
}

int main(int argc, char* argv[]) {
    --argc; ++argv;
    auto const f = read_file(argv[0]);
    size_t read_count = std::stoull(argv[1], nullptr, 0);
    if (!read_count) {
        --read_count;
    }
    try {
        while (read_count) {
            --read_count;
            auto next_msg = f();
            if (!next_msg.has_value()) {
                std::cerr << "Detected end of file" << std::endl;
                continue;
            }
            switch (next_msg->message_type_.enumerate()) {
                // using Enum = SoupbinTcp_MessageType::Enum;
                case SoupbinTcp_MessageType::Enum::SequencedData:
                    std::cerr << "ok" << std::endl;
                    break;
                default:
                    std::cerr << "ng" << std::endl;
                    break;
            }
        }
    } catch (std::exception const& excp) {
        std::cerr << excp.what() << std::endl; // goes to final suspend point
        f.~Generator();
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
