#include <fstream>
#include <stdexcept>
#include <format>
#include <utility>
#include <string_view>
#include <atomic>
#include <coroutine>
#include <iostream>
#include <vector>
#include <cassert>
#include <cctype> 
#include <memory>


#include "char_enum.hpp"


struct SbtcpMessage {
    inline SbtcpMessage(void) = default;
    inline SbtcpMessage(SbtcpMessage const& other) { operator=(other); }
    inline SbtcpMessage& operator=(SbtcpMessage const& other) {
        message_type_ = other.message_type_;
        data_ = data_;
        return *this;
    }
    inline SbtcpMessage& operator=(SbtcpMessage&& other) {
        message_type_ = other.message_type_;
        take(std::move(other.data_));
        return *this;
    }
    inline SbtcpMessage(SbtcpMessage&& other) {
        message_type_ = other.message_type_; // or should this be swap since this may cause a memory leak?
        take(std::move(other.data_));
    }
    inline SbtcpMessage& take(std::vector<unsigned char>&& other) {
        if (&(this->data_) != &other) {
            std::cerr << "data moved" << std::endl;
            data_.~vector(); // do i need to delete this to prevent memory leak?
                             // i dont think so because this should be dealt with
                             // by the std::vector move assignment operator
            data_ = std::move(other); // Or should this be swap?
        }
        return *this;
    }
    SoupbinTcpMessageType message_type_{};
    std::vector<unsigned char> data_{};
};

template <typename T>
struct Generator {
    struct promise_type {
        auto get_return_object(void) noexcept { return Generator{ *this }; }
        std::suspend_always initial_suspend(void) const noexcept { return {}; }
        std::suspend_always final_suspend(void) const noexcept { return {}; }
        std::suspend_always yield_value([[maybe_unused]] bool a) noexcept {
            result = std::monostate{};
            return {};
        }
        std::suspend_always yield_value(void) noexcept {
            result = std::monostate{};
            return {};
        }
        std::suspend_always yield_value(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
            result = value;
            return {};
        }
        std::suspend_always yield_value(T&& value) {
            result = std::move(value);
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
        bool has_value(void) const noexcept {
            return std::holds_alternative<T>(result);
        }
        void reset(void) noexcept {
            result = std::monostate{};
        }
        std::unique_ptr<T> give(void) {
            if (!has_value())
                return nullptr;
            auto res = std::make_unique<T>(std::move(std::get<T>(result)));
            result = std::monostate{};
            return res;
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
    bool has_value(void) const {
        return coro.promise().has_value();
    }
    bool advance(void) const {
        coro.promise().reset();
        coro(); // same as coro.resume() : reach next suspension point and return control
        return has_value();
    }
    auto const& get(void) const {
        return coro.promise().getValue();
    }
    std::unique_ptr<T> take(void) {
        return coro.promise().give();
    }
private:
    explicit Generator(promise_type& promise) noexcept :
        coro{ std::coroutine_handle<promise_type>::from_promise(promise) }
    {}

    std::coroutine_handle<promise_type> coro;
};


Generator<SbtcpMessage> read_file(std::string const& path) {
    [[maybe_unused]] size_t message_start_byte = 0;
    [[maybe_unused]] size_t total_bytes_read = 0;
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
                co_yield false; // co_yield is equivalent to co_await promise.yield_value(expression);
            else
                break;
        }
    }
    unsigned len = len_str[0] * 256 + len_str[1];

// Read Sbtcp message type
    unsigned char msgtype;
    while (true) {
        if (!read_one_char(msgtype))
            co_yield false;
        else
            break;
    }
    SbtcpMessage data_container{};
    data_container.message_type_.unpack(msgtype);
    assert(len >= 1);
    --len; // Remove Sbtcp message type char
    std::cerr << std::format("DEBUG: {{ start={}, length={}, msgtype={} }}\n", message_start_byte, len + sizeof(len_str), data_container.message_type_);

// Read the rest of the message
    std::vector<unsigned char> packet(len);

    for (size_t i = 0; i < len; ++i) {
        while (true) {
            if (!read_one_char(packet[i]))
                co_yield false;
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
    std::cerr << "before .copy()" << std::endl;
    data_container.take(std::move(packet));
    std::cerr << "before co_yield()" << std::endl;
    co_yield std::move(data_container);

    goto start_cycle;
}

static
void on_sequenced_data([[maybe_unused]] std::unique_ptr<SbtcpMessage> data) {
    std::cerr << "DEBUG: sequenced message; parse" << std::endl;
    std::string s{"PAYLOAD:"};
    for (auto const& c : data->data_) {
         s += std::format("{:02x}|", (unsigned)c);
    }
    s.pop_back();
    std::cerr << s << std::endl;
}

void take_demo(std::string const& path, size_t max_lines) {
    auto f = read_file(path);
    try {
        while (max_lines) {
            throw std::runtime_error("test");
            --max_lines;
            if (!f.advance()) {
                std::cerr << "Detected end of file" << std::endl;
                break;
            }
            // auto next_msg = f.get();
            // assert(f.has_value());
            // auto const dispatch = next_msg.message_type_.enumerate();
            std::cerr << "before f.take()" << std::endl;
            auto next_msg = f.take(); // take resource from the promise
            assert(!f.has_value());   // there is no more value here
            auto const dispatch = next_msg->message_type_.enumerate();

            switch (dispatch) {
                case SoupbinTcp_MessageType::Enum::SequencedData:
                    on_sequenced_data(std::move(next_msg));
                    assert(!next_msg);
                    break;
                case SoupbinTcp_MessageType::Enum::EndOfSession:
                    // std::cerr << "DEBUG: no more data expected from here on out" << std::endl;
                    break;
                default:
                    // std::cerr << "DEBUG: not a sequenced message; ignore" << std::endl;
                    break;
            }
        }
    } catch (std::exception const& excp) {
        std::cerr << excp.what() << std::endl; // goes to final suspend point
        f.~Generator();
        throw excp;
    }
}
// void get_demo(std::string path, size_t max_lines) {
// 
// }
int main(int argc, char* argv[]) {
    --argc; ++argv;
    // auto const f = read_file(argv[0]);
    size_t read_count = std::stoull(argv[1], nullptr, 0);
    if (!read_count) {
        --read_count;
    }
    try {
        take_demo(argv[0], read_count);
    } catch (std::exception const& excp) {
        std::cerr << "caught exception: " << excp.what() << std::endl;;
    }
    exit(EXIT_SUCCESS);
}
