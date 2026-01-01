#ifndef RESULT_H
#define RESULT_H

#include <utility>
#include <stdexcept>

/**
 * @brief Rust-style Result<T, E> type for error handling
 * 
 * Provides a type-safe way to return either a success value or an error.
 * Usage:
 *   auto result = someFunction();
 *   if (result.isOk()) {
 *       auto value = result.value();
 *   } else {
 *       auto error = result.error();
 *   }
 */
template<typename T, typename E>
class Result {
public:
    static Result ok(T value) { 
        Result r;
        r.m_isOk = true;
        r.m_value = std::move(value);
        return r;
    }
    
    static Result err(E error) { 
        Result r;
        r.m_isOk = false;
        r.m_error = std::move(error);
        return r;
    }
    
    bool isOk() const { return m_isOk; }
    bool isErr() const { return !m_isOk; }
    
    T& value() { 
        if (!m_isOk) {
            throw std::runtime_error("Called value() on an error Result");
        }
        return m_value; 
    }
    
    const T& value() const { 
        if (!m_isOk) {
            throw std::runtime_error("Called value() on an error Result");
        }
        return m_value; 
    }
    
    E& error() { 
        if (m_isOk) {
            throw std::runtime_error("Called error() on an ok Result");
        }
        return m_error; 
    }
    
    const E& error() const { 
        if (m_isOk) {
            throw std::runtime_error("Called error() on an ok Result");
        }
        return m_error; 
    }
    
    T valueOr(T defaultValue) const {
        return m_isOk ? m_value : defaultValue;
    }
    
private:
    bool m_isOk;
    T m_value;
    E m_error;
    
    Result() : m_isOk(false), m_value(), m_error() {}
};

/**
 * @brief Specialization for void success type
 * 
 * Used when an operation doesn't return a value on success,
 * only indicates success or failure.
 */
template<typename E>
class Result<void, E> {
public:
    static Result ok() { 
        Result r;
        r.m_isOk = true;
        return r;
    }
    
    static Result err(E error) { 
        Result r;
        r.m_isOk = false;
        r.m_error = std::move(error);
        return r;
    }
    
    bool isOk() const { return m_isOk; }
    bool isErr() const { return !m_isOk; }
    
    E& error() { 
        if (m_isOk) {
            throw std::runtime_error("Called error() on an ok Result");
        }
        return m_error; 
    }
    
    const E& error() const { 
        if (m_isOk) {
            throw std::runtime_error("Called error() on an ok Result");
        }
        return m_error; 
    }
    
private:
    bool m_isOk;
    E m_error;
    
    Result() : m_isOk(false), m_error() {}
};

#endif // RESULT_H
