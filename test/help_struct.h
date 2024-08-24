#include <cstdio>
#include <cstring>
#include <iostream>
struct String {
    String() : m_data(nullptr), m_size(0) {}
    String(const char *str) {
        m_size = strlen(str);
        m_data = new char[m_size];
        memcpy(m_data, str, m_size * sizeof(char));
    }
    String(const String &other)
        : m_data(new char[other.m_size]), m_size(other.m_size) {
        memcpy(m_data, other.m_data, m_size * sizeof(char));
    }
    String &operator=(const String &other) {
        if (this != &other) {
            if (this->m_data != nullptr) {
                delete[] m_data;
            }
            m_size = other.m_size;
            m_data = new char[m_size];
            memcpy(m_data, other.m_data, m_size * sizeof(char));
        }
        return *this;
    }
    String(String &&other) : m_data(other.m_data), m_size(other.m_size) {
        other.m_data = nullptr;
        other.m_size = 0;
    }
    String &operator=(String &&other) {
        if (this != &other) {
            if (this->m_data != nullptr) {
                delete[] m_data;
            }
            m_size = other.m_size;
            m_data = other.m_data;
            other.m_data = nullptr;
            other.m_size = 0;
        }
        return *this;
    }
    ~String() {
        m_size = 0;
        delete[] m_data;
    }

    void print() const { std::cout << m_data << std::endl; }
    unsigned int size() const { return m_size; }

    friend bool operator==(const String &lhs, const String &rhs);

  private:
    char *m_data;
    unsigned int m_size;
};

bool operator==(const String &lhs, const String &rhs) {
    if (lhs.m_size != rhs.m_size) {
        return false;
    }
    for (unsigned int i = 0; i < lhs.m_size; i++) {
        if (*(lhs.m_data + i) != *(rhs.m_data + i)) {
            printf("%x %x\n", static_cast<int>(*(lhs.m_data + i)),
                   static_cast<int>(*(lhs.m_data + i)));
            return false;
        }
    }
    return true;
}
