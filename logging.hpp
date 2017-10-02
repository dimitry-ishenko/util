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

// send logs to console
//
// When enabled, debug & info messages will be
// logged to std::cout and warn & error messages
// will be logged to std::cerr.
//
bool send_to_console() noexcept;
void send_to_console(bool) noexcept;

// send logs to syslog
//
bool send_to_syslog() noexcept;
void send_to_syslog(bool) noexcept;

////////////////////////////////////////////////////////////////////////////////
// log level
enum class level { dbg, info, warn, err };

////////////////////////////////////////////////////////////////////////////////
// logging stream
//
// Instances of this class are returned by
// dbg(), info(), warn(), err() and log() functions.
//
class stream : private std::ostringstream
{
    using base = std::ostringstream;

public:
    ////////////////////
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
namespace logging
{

inline auto  dbg() { return stream("", level::dbg ); }
inline auto info() { return stream("", level::info); }
inline auto warn() { return stream("", level::warn); }
inline auto  err() { return stream("", level::err ); }

// log with user-defined level
inline auto log(level l) { return stream("", l); }

}

////////////////////////////////////////////////////////////////////////////////
// Logger class
//
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

    auto  dbg() { return stream(name_, level::dbg ); }
    auto info() { return stream(name_, level::info); }
    auto warn() { return stream(name_, level::warn); }
    auto  err() { return stream(name_, level::err ); }

    // log with user-defined level
    auto log(util::level level) { return stream(name_, level); }

private:
    ////////////////////
    std::string name_;
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
