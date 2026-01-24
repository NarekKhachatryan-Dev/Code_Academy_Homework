#include <initializer_list>

template <typename T>
class vector {
private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;

public:
    vector() : m_data(nullptr), m_size(0), m_capacity(0) {}

    vector(size_t capacity) 
        : m_data(new T[capacity]), m_size(0), m_capacity(capacity) {}

    vector(std::initializer_list<T> init) 
        : m_data(new T[init.size()]), m_size(init.size()), m_capacity(init.size()) {
        size_t index = 0;
        for (const T& item : init) {
            m_data[index++] = item;
        }
    }

    vector(const vector& other) : m_size(other.m_size), m_capacity(other.m_capacity) {
        m_data = new T[m_capacity];
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    }

    vector(vector&& other) noexcept 
        : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity) {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            delete[] m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_data = new T[m_capacity];
            for (size_t i = 0; i < m_size; ++i) {
                m_data[i] = other.m_data[i];
            }
        }
        return *this;
    }
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            delete[] m_data;
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    ~vector() {
        delete[] m_data;
    }

    size_t size() const {
        return m_size;
    }

    size_t capacity() const {
        return m_capacity;
    }

    void reserve(size_t new_capacity) {
        if(new_capacity > m_capacity) {
            T* new_data = new T[new_capacity];
            for(size_t i = 0; i < m_size; ++i) {
                new_data[i] = m_data[i];
            }
            delete[] m_data;
            m_data = new_data;
            m_capacity = new_capacity;
        }
    }

    void resize(size_t new_size) {
        if(new_size == m_size) {
            return;
        }
        if(new_size > m_capacity) {
            reserve(new_size);
        } else if(new_size < m_size) {
            for(size_t i = new_size; i < m_size; ++i) {
                m_data[i].~T();
            }
        }
        m_size = new_size;
    }

    void push_back(const T& value) {
        if(m_size == m_capacity) {
            size_t new_capacity = (m_capacity == 0) ? 1 : m_capacity * 2;
            T* new_data = new T[new_capacity];

            for (size_t i = 0; i < m_size; i++)
            {
                new_data[i] = m_data[i];
            }
            delete[] m_data;
            m_data = new_data;
            m_capacity = new_capacity;
            
        }
        m_data[m_size++] = value;
    }

    T& operator[](size_t index) {
        return m_data[index];
    }

    T& operator[](size_t index) const {
        return m_data[index];
    }

    void clear() {
        m_size = 0;
    }

    void pop_back() {
        if (m_size > 0) {
            --m_size;
        }
    }

    bool empty() const {
        return m_size == 0;
    }

    bool operator==(const vector& other) const {
        if (m_size != other.m_size) {
            return false;
        }
        for (size_t i = 0; i < m_size; ++i) {
            if (m_data[i] != other.m_data[i]) {
                return false;
            }
        }
        return true;
    }

    T& front() {
        return m_data[0];
    }

    T& back() {
        return m_data[m_size - 1];
    }

};