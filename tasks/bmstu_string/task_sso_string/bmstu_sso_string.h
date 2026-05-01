#pragma once

#include <exception>
#include <iostream>
#include <cstring>

namespace bmstu
{
template <typename T>
class basic_string;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template <typename T>
class basic_string
{
   private:
	static constexpr size_t SSO_CAPACITY =
		(sizeof(T*) + sizeof(size_t) + sizeof(size_t)) / sizeof(T) - 1;

	struct LongString
	{
		T* ptr;
		size_t size;
		size_t capacity;
	};

	struct ShortString
	{
		T buffer[SSO_CAPACITY + 1];
		unsigned char size;
	};

	union Data
	{
		LongString long_str;
		ShortString short_str;
	};

	Data data_;
	bool is_long_;

	bool is_long() const { return is_long_; }

	T* get_ptr() { return is_long_ ? data_.long_str.ptr : nullptr; }

	const T* get_ptr() const { return is_long_ ? data_.long_str.ptr : nullptr; }

	size_t get_size() const { return is_long_ ? data_.long_str.size : data_.short_str.size; }

	size_t get_capacity() const { return is_long_ ? data_.long_str.capacity : SSO_CAPACITY;}

   public:
	basic_string() : is_long_(false){
		data_.short_str.size = 0;
		data_.short_str.buffer[0] = T();
	}

	basic_string(size_t size) : is_long_(false){
		if(size < SSO_CAPACITY){
			data_.short_str.size = static_cast<unsigned char>(size);
		} else {
			is_long_ = true;
			data_.long_str.size = size;
			data_.long_str.capacity = size;
			data_.long_str.ptr = new T[size + 1]{};
		}
	}

	basic_string(std::initializer_list<T> il) : is_long_(false) {
		size_t i = 0;
		for(const T& c : il){
			if(i < SSO_CAPACITY){
				data_.short_str.buffer[i] = c;
			}
			++i;
		}
		data_.short_str.size = static_cast<unsigned char>(i);
	}

	basic_string(const T* c_str) : basic_string(strlen_(c_str)) {
		size_t i = 0;
		while(c_str[i] != '\0'){
			(*this)[i++] =  c_str[i];
		}
	}

	basic_string(const basic_string& other) : is_long_(other.is_long_) {
		if(is_long_){
			data_.long_str.ptr = new T[other.data_.long_str.size + 1];
			std::memcpy(data_.long_str.ptr, other.data_.long_str.ptr, other.data_.long_str.size * sizeof(T));
			data_.long_str.size = other.data_.long_str.size;
			data_.long_str.capacity = other.data_.long_str.capacity;
		}else{
			data_.short_str.size = other.data_.short_str.size;
			std::memcpy(data_.short_str.buffer, other.data_.short_str.buffer,other.data_.short_str.size * sizeof(T)); 
		}
	}

	basic_string(basic_string&& dying) noexcept : is_long_(dying.is_long_){
		if(is_long_){
			data_.long_str.ptr = dying.data_.long_str.ptr;
			data_.long_str.size = dying.data_.long_str.size;
			data_.long_str.capacity = dying.data_.long_str.capacity;
			dying.data_.long_str.ptr = nullptr;
		}else{
			data_.short_str = std::move(dying.data_.short_str);
		}
	}

	~basic_string() {
		if(is_long_){
			delete[] data_.long_str.ptr;
		}
	}

	const T* c_str() const { 
		return is_long() ? data_.long_str.ptr : data_.short_str.buffer;
	}

	size_t size() const { 
		return get_size(); 
	}

	bool is_using_sso() const { 
		return !is_long(); 
	}

	size_t capacity() const { 
		return get_capacity(); }

	basic_string& operator=(basic_string&& other) noexcept { 
		if(this != &other){
			if(is_long_){
				delete[] data_.long_str.ptr;
			}
			is_long_ = other.is_long_;
			if(is_long_){
				data_.long_str.ptr = other.data_.long_str.ptr;
				data_.long_str.size = other.data_.long_str.size;
				data_.long_str.capacity = other.data_.long_str.capacity;
				other.data_.long_str.ptr = nullptr;
			}else{
				data_.short_str = std::move(other.data_.short_str);
			}
		}return *this;
	}

	basic_string& operator=(const T* c_str) { 
		return *this; 
	}

	basic_string& operator=(const basic_string& other) { 
		if(this != &other){
			if(is_long_){
				delete[] data_.long_str.ptr;
			}
			is_long_ = other.is_long_;
			if(is_long_){
				data_.long_str.ptr = new T[other.data_.long_str.size + 1];
				std::memcpy(data_.long_str.ptr,other.data_.long_str.ptr, other.data_.long_str.size * sizeof(T));
				data_.long_str.size = other.data_.long_str.size;
				data_.long_str.capacity = other.data_.long_str.capacity;
			}else{
				data_.short_str.size = other.data_.short_str.size;
				std::memcpy(data_.short_str.buffer,other.data_.short_str.buffer, other.data_.short_str.size * sizeof(T));
			}
		 }return *this;
	}

	friend basic_string<T> operator+(const basic_string<T>& left,
									 const basic_string<T>& right)
	{
		return {};
	}

	template <typename S>
	friend S& operator<<(S& os, const basic_string& obj)
	{
		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, basic_string& obj)
	{
		return is;
	}

	basic_string& operator+=(const basic_string& other) { return *this; }

	basic_string& operator+=(T symbol) { return *this; }

	T& operator[](size_t index) noexcept
	{
		static T dummy;
		return dummy;
	}

	T& at(size_t index) { throw std::out_of_range("Wrong index"); }

	T* data() { 
		return is_long() ? data_.long_str.ptr : data_.short_str.buffer; 
	}

   private:
	static size_t strlen_(const T* str) { 
		size_t length = 0;
		while(str[length]){
			++length;
		}
		return length; 
	}


	void clean_() {
		if(is_long_){
			delete[] data_.long_str.ptr;
		}
	}
};
}  // namespace bmstu
