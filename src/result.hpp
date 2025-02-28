#pragma once

template <typename S, typename T>
class Result {
private:
    std::optional<S> value_;
    std::optional<T> error_;

public:
    
    // Constructor for success case
    explicit Result(S value) : value_(std::move(value)) {}
    
    // Constructor for error case
    explicit Result(T error) : error_(std::move(error)) {}
    
    // Static factory methods (similar to Rust's Ok and Err)
    static Result<S, T> Ok(S value) {
        return Result(std::move(value));
    }
    
    static Result<S, T> Error(T error) {
        return Result(std::move(error));
    }
    
    bool is_error() const {
        return error_.has_value();
    }

    S unwrap() const {
        if (!is_error()) {
            return value_.value();
        }
        throw std::runtime_error(error_.value());
    }

    T unwrap_error() const {
        if (is_error()) {
            return error_.value();
        }
        throw std::runtime_error("attempt to unwrap the error of a Result which is Ok");
    }
};
