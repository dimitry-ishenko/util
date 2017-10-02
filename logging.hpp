////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef UTIL_LOGGING_HPP
#define UTIL_LOGGING_HPP

////////////////////////////////////////////////////////////////////////////////
#include <sstream>
#include <string>
#include <utility>

////////////////////////////////////////////////////////////////////////////////
namespace util
{

////////////////////////////////////////////////////////////////////////////////
// get/set whether to log debug messages
//
// unless explicitly set to false,
// debug messages will also be logged,
// if the environment variable DEBUG is set
//
bool send_debug() noexcept;
void send_debug(bool) noexcept;

// get/set whether to log to console
//
// if enabled, debug & info messages will be logged to std::cout
// and warn & error messages will be logged to std::cerr
//
bool send_to_console() noexcept;
void send_to_console(bool) noexcept;

// get/set whether to log to syslog
//
bool send_to_syslog() noexcept;
void send_to_syslog(bool) noexcept;

////////////////////////////////////////////////////////////////////////////////
// log level
enum class level { debug, info, warn, error };

////////////////////////////////////////////////////////////////////////////////
// logging stream
//
// instances of this class are returned by
// debug(), info(), warn() and error() functions;
//
class stream : private std::ostringstream
{
    using base = std::ostringstream;

public:
    ////////////////////
    explicit stream(level l) : level_(l) { }
    stream(const std::string&, level);
    ~stream();

    stream(const stream&) = delete;
    stream(stream&& rhs) : base(std::move(rhs)), level_(rhs.level_) { }

    stream& operator=(const stream&) = delete;
    stream& operator=(stream&& rhs)
    { *static_cast<base*>(this) = std::move(rhs); level_ = rhs.level_; return *this; }

    ////////////////////
    template<typename T>
    stream& operator<<(T&& value) &
    { *static_cast<base*>(this) << std::forward<T>(value); return *this; }

    template<typename T>
    stream&& operator<<(T&& value) &&
    { *static_cast<base*>(this) << std::forward<T>(value); return std::move(*this); }

    stream& write(const char* s, std::streamsize n) &
    { base::write(s, n); return *this; }

    stream&& write(const char* s, std::streamsize n) &&
    { base::write(s, n); return std::move(*this); }

private:
    ////////////////////
    level level_;
};

////////////////////////////////////////////////////////////////////////////////
// logging functions
//
inline auto debug() { return stream(level::debug); }
inline auto  info() { return stream(level::info ); }
inline auto  warn() { return stream(level::warn ); }
inline auto error() { return stream(level::error); }

namespace logging
{

using util::debug;
using util::info;
using util::warn;
using util::error;

}

////////////////////////////////////////////////////////////////////////////////
class logger
{
protected:
    ////////////////////
    explicit logger(std::string name = std::string()) { this->name(name); }
    virtual ~logger() noexcept { }

    logger(const logger&) = default;
    logger(logger&&) = default;

    logger& operator=(const logger&) = default;
    logger& operator=(logger&&) = default;

    ////////////////////
    auto const& name() const noexcept { return name_; }
    void name(std::string name) noexcept { name_ = std::move(name); }

    auto debug() { return stream(name_, level::debug); }
    auto  info() { return stream(name_, level::info ); }
    auto  warn() { return stream(name_, level::warn ); }
    auto error() { return stream(name_, level::error); }

private:
    ////////////////////
    std::string name_;
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
