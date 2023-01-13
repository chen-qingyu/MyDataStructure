/**
 * @file List.hpp
 * @author 青羽 (chen_qingyu@qq.com, https://chen-qingyu.github.io/)
 * @brief List template class, implemented by array list.
 * @version 1.0
 * @date 2023.01.05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef LIST_HPP
#define LIST_HPP

#include <climits>          // INT_MAX
#include <initializer_list> // std::initializer_list
#include <ostream>          // std::ostream

#include "ListIterator.hpp"

#include "common/check_bounds.hpp"
#include "common/check_empty.hpp"
#include "common/check_full.hpp"
#include "common/swap.hpp"

namespace mdspp
{

/**
 * @brief List template class, implemented by array list.
 *
 * @tparam T the type of elements in the list
 */
template <typename T>
class List
{
    friend class String;

private:
    // Number of elements.
    int size_;

    // Available capacity.
    int capacity_;

    // Pointer to the data.
    T* data_;

    // Initial capacity.
    static const int INIT_CAPACITY = 4; // for easily test expand capacity

    // Maximum capacity.
    static const int MAX_CAPACITY = INT_MAX - 1; // - 1 to prevent boundary subscript overflow

    // Expand capacity safely.
    void expand_capacity()
    {
        // double capacity until MAX_CAPACITY
        capacity_ = (capacity_ < MAX_CAPACITY / 2) ? capacity_ * 2 : MAX_CAPACITY;

        // move data
        T* tmp = new T[capacity_];
        for (int i = 0; i < size_; ++i)
        {
            tmp[i] = data_[i];
        }
        delete[] data_;
        data_ = tmp;
    }

public:
    /*
     * Constructor / Destructor
     */

    /**
     * @brief Construct a new list object.
     */
    List()
        : size_(0)
        , capacity_(INIT_CAPACITY)
        , data_(new T[capacity_])
    {
    }

    /**
     * @brief Create a list based on the given initializer list.
     *
     * @param il initializer list
     */
    List(const std::initializer_list<T>& il)
        : size_((int)il.size())
        , capacity_(size_ >= INIT_CAPACITY ? size_ : INIT_CAPACITY)
        , data_(new T[capacity_])
    {
        for (int i = 0; i < size_; ++i)
        {
            data_[i] = *(il.begin() + i);
        }
    }

    /**
     * @brief Copy constructor.
     *
     * @param that another list
     */
    List(const List<T>& that)
        : size_(that.size_)
        , capacity_(that.capacity_)
        , data_(new T[capacity_])
    {
        for (int i = 0; i < size_; ++i)
        {
            data_[i] = that.data_[i];
        }
    }

    /**
     * @brief Move constructor.
     *
     * @param that another list
     */
    List(List<T>&& that)
        : size_(that.size_)
        , capacity_(that.capacity_)
        , data_(that.data_)
    {
        that.size_ = 0;
        that.capacity_ = INIT_CAPACITY;
        that.data_ = new T[that.capacity_];
    }

    /**
     * @brief Destroy the list object.
     */
    ~List()
    {
        delete[] data_; // the data pointer is guaranteed to be valid
    }

    /*
     * Access
     */

    /**
     * @brief Return the reference to the element at the specified position in the list.
     *
     * Index can be negative, like Python's list: list[-1] gets the last element.
     *
     * @param index index of the element to return (-size() <= index < size())
     * @return reference to the element at the specified position in the list
     */
    T& operator[](int index)
    {
        common::check_bounds(index, -size_, size_);

        return index >= 0 ? data_[index] : data_[size_ + index];
    }

    /**
     * @brief Return the const reference to element at the specified position in the list.
     *
     * Index can be negative, like Python's list: list[-1] gets the last element.
     *
     * @param index index of the element to return (-size() <= index < size())
     * @return const reference to the element at the specified position in the list
     */
    const T& operator[](int index) const
    {
        common::check_bounds(index, -size_, size_);

        return index >= 0 ? data_[index] : data_[size_ + index];
    }

    /*
     * Iterator
     */

    /**
     * @brief Return an iterator to the first element of the list.
     *
     * If the list is empty, the returned iterator will be equal to end().
     *
     * @return iterator to the first element
     */
    ListIterator<T> begin() const
    {
        return ListIterator<T>(data_);
    }

    /**
     * @brief Return an iterator to the element following the last element of the list.
     *
     * This element acts as a placeholder, attempting to access it results in undefined behavior.
     *
     * @return iterator to the element following the last element
     */
    ListIterator<T> end() const
    {
        return ListIterator<T>(data_ + size_); // not nullptr, because size_ <= capacity_
    }

    /*
     * Assignment
     */

    /**
     * @brief Copy assignment operator.
     *
     * @param that another list
     * @return self reference
     */
    List& operator=(const List<T>& that)
    {
        if (this != &that)
        {
            size_ = that.size_;
            capacity_ = that.capacity_;

            delete[] data_;
            data_ = new T[capacity_];
            for (int i = 0; i < size_; ++i)
            {
                data_[i] = that.data_[i];
            }
        }

        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param that another list
     * @return self reference
     */
    List& operator=(List<T>&& that)
    {
        if (this != &that)
        {
            delete[] data_;

            size_ = that.size_;
            capacity_ = that.capacity_;
            data_ = that.data_;

            that.size_ = 0;
            that.capacity_ = INIT_CAPACITY;
            that.data_ = new T[that.capacity_];
        }

        return *this;
    }

    /*
     * Examination (will not change the object itself)
     */

    /**
     * @brief Return the number of elements in the list.
     *
     * @return the number of elements in the list
     */
    int size() const
    {
        return size_;
    }

    /**
     * @brief Return true if the list contains no elements.
     *
     * @return true if the list contains no elements
     */
    bool is_empty() const
    {
        return size_ == 0;
    }

    /**
     * @brief Check whether two lists are equal.
     *
     * @param that another list
     * @return true if two lists are equal
     */
    bool operator==(const List<T>& that) const
    {
        if (size_ != that.size_)
        {
            return false;
        }

        for (int i = 0; i < size_; ++i)
        {
            if (data_[i] != that.data_[i])
            {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Check whether two lists are not equal.
     *
     * @param that another list
     * @return true if two lists are not equal
     */
    bool operator!=(const List<T>& that) const
    {
        return !(*this == that);
    }

    /**
     * @brief Return the index of the first occurrence of the specified element in the list (at or after index start and before index stop).
     *
     * Or -1 if the list does not contain the element (in the specified range).
     *
     * @param element element to search for
     * @param start at or after index start (default 0)
     * @param stop before index stop (default size())
     * @return the index of the first occurrence of the specified element in the list, or -1 if the list does not contain the element
     */
    int find(const T& element, int start = 0, int stop = MAX_CAPACITY + 1) const
    {
        stop = stop > size_ ? size_ : stop;
        for (int i = start; i < stop; ++i)
        {
            if (data_[i] == element)
            {
                return i;
            }
        }

        return -1;
    }

    /**
     * @brief Return true if the list contains the specified element.
     *
     * @param element element whose presence in the list is to be tested
     * @param start at or after index start (default 0)
     * @param stop before index stop (default size())
     * @return true if the list contains the specified element
     */
    bool contains(const T& element, int start = 0, int stop = MAX_CAPACITY + 1) const
    {
        return find(element, start, stop) != -1;
    }

    /**
     * @brief Get the smallest item of the list.
     *
     * @return the smallest item
     */
    T min() const
    {
        common::check_empty(size_);

        T smallest = data_[0];
        for (int i = 0; i < size_; i++)
        {
            if (smallest > data_[i])
            {
                smallest = data_[i];
            }
        }

        return smallest;
    }

    /**
     * @brief Get the largest item of the list.
     *
     * @return the largest item
     */
    T max() const
    {
        common::check_empty(size_);

        T largest = data_[0];
        for (int i = 0; i < size_; i++)
        {
            if (largest < data_[i])
            {
                largest = data_[i];
            }
        }

        return largest;
    }

    /**
     * @brief Count the total number of occurrences of the specified element in the list.
     *
     * @param element the specified element
     * @return the total number of occurrences of the specified element in the list
     */
    int count(const T& element) const
    {
        int counter = 0;
        for (int i = 0; i < size_; i++)
        {
            if (data_[i] == element)
            {
                counter++;
            }
        }
        return counter;
    }

    /*
     * Manipulation (will change the object itself)
     */

    /**
     * @brief Insert the specified element at the specified position in the list.
     *
     * @param index index at which the specified element is to be inserted (-size() <= index <= size())
     * @param element element to be inserted
     */
    void insert(int index, const T& element)
    {
        // check
        common::check_full(size_, MAX_CAPACITY);

        common::check_bounds(index, -size_, size_ + 1);

        // expand capacity
        if (size_ == capacity_)
        {
            expand_capacity();
        }

        // index
        index = index >= 0 ? index : index + size_;
        for (int i = size_; i > index; --i)
        {
            data_[i] = data_[i - 1];
        }

        // insert
        data_[index] = element; // copy assignment on T

        // resize
        ++size_;
    }

    /**
     * @brief Remove the element at the specified position in the list.
     *
     * @param index the index of the element to be removed (-size() <= index < size())
     * @return the removed element
     */
    T remove(int index)
    {
        // check
        common::check_empty(size_);

        common::check_bounds(index, -size_, size_);

        // get element
        index = index >= 0 ? index : index + size_;
        T element = data_[index];

        // index and remove
        for (int i = index + 1; i < size_; ++i)
        {
            data_[i - 1] = data_[i];
        }

        // resize
        --size_;

        // return element
        return element;
    }

    /**
     * @brief Append the specified element to the end of the list.
     *
     * @param element element to be appended to the list
     * @return self reference
     */
    List& operator+=(const T& element)
    {
        insert(size_, element);

        return *this;
    }

    /**
     * @brief Append the specified list to the end of the list.
     *
     * @param list list to be appended to the list
     * @return self reference
     */
    List& operator+=(const List& list)
    {
        if (this == &list)
        {
            List saved_list = list; // save list, for valid addr when expand capacity when this == &list
            for (int i = 0; i < saved_list.size_; i++)
            {
                *this += saved_list.data_[i];
            }
        }
        else
        {
            for (int i = 0; i < list.size_; i++)
            {
                *this += list.data_[i];
            }
        }

        return *this;
    }

    /**
     * @brief Remove the first occurrence of the specified element from the list, if it is present.
     *
     * If the list does not contain the element, it is unchanged.
     *
     * @param element element to be removed
     * @return self reference
     */
    List& operator-=(const T& element)
    {
        int index = find(element);
        if (index != -1)
        {
            remove(index);
        }

        return *this;
    }

    /**
     * @brief Add the list to itself a certain number of times.
     *
     * @param times times to repeat
     * @return self reference
     */
    List& operator*=(int times)
    {
        if (times < 0)
        {
            throw std::runtime_error("ERROR: Times to repeat cannot be less than zero.");
        }

        List list;
        for (int i = 0; i < times; i++)
        {
            list += *this;
        }
        *this = list;

        return *this;
    }

    /**
     * @brief Remove all of the elements from the list.
     */
    void clear()
    {
        if (size_ != 0)
        {
            size_ = 0;
            capacity_ = INIT_CAPACITY;
            delete[] data_;
            data_ = new T[capacity_];
        }
    }

    /**
     * @brief Perform the given action for each element of the list.
     *
     * @tparam F function
     * @param action a function that to be performed for each element
     */
    template <typename F>
    void traverse(F action)
    {
        for (int i = 0; i < size_; ++i)
        {
            action(data_[i]);
        }
    }

    /**
     * @brief Reverse the list in place.
     */
    void reverse()
    {
        for (int i = 0, j = size_ - 1; i < j; ++i, --j)
        {
            common::swap(data_[i], data_[j]);
        }
    }

    /**
     * @brief Eliminate duplicate elements of the list.
     */
    void uniquify()
    {
        for (int i = 0; i < size_; i++)
        {
            while (count(data_[i]) > 1)
            {
                *this -= data_[i];
            }
        }
    }

    /**
     * @brief Sort the list according to the order induced by the specified comparator.
     *
     * The sort is stable: the method must not reorder equal elements.
     *
     * @param comparator a function that performs compare
     */
    void sort(bool (*comparator)(const T& e1, const T& e2) = [](const T& e1, const T& e2)
              { return e1 < e2; })
    {
        // for simplicity, bubble sort is usually enough

        bool swapped;
        for (int i = 0; i < size_ - 1; i++)
        {
            swapped = false;
            for (int j = 0; j < size_ - i - 1; j++)
            {
                if (comparator(data_[j + 1], data_[j]))
                {
                    common::swap(data_[j], data_[j + 1]);
                    swapped = true;
                }
            }
            if (swapped == false)
            {
                break;
            }
        }
    }

    /**
     * @brief Swap the contents of two lists.
     *
     * @param that second list
     */
    void swap(List& that)
    {
        common::swap(size_, that.size_);
        common::swap(capacity_, that.capacity_);
        common::swap(data_, that.data_);
    }

    /*
     * Production (will produce new object)
     */

    /**
     * @brief Return slice of the list from start to stop with certain step.
     *
     * Index and step length can be negative.
     *
     * @param start start index (included)
     * @param stop stop index (excluded)
     * @param step step length (default 1)
     * @return the slice of the list
     */
    List slice(int start, int stop, int step = 1) const
    {
        // although I have implemented the Python-like slice function (index can exceed the size),
        // I finally decided to check the index for the simplicity of source code and the security of use, and consistent with the operator[]

        // check
        if (step == 0)
        {
            throw std::runtime_error("ERROR: Slice step cannot be zero.");
        }

        common::check_bounds(start, -size_, size_);
        common::check_bounds(stop, -size_ - 1, size_ + 1);

        // convert
        start = start < 0 ? start + size_ : start;
        stop = stop < 0 ? stop + size_ : stop;

        // copy
        List list;
        for (int i = start; (step > 0) ? (i < stop) : (i > stop); i += step)
        {
            list += data_[i];
        }

        return list;
    }

    /**
     * @brief Generate a new list and append the specified element to the end of the list.
     *
     * @param element element to be appended to the list
     * @return the generated list
     */
    List operator+(const T& element) const
    {
        List new_list = *this;
        return new_list += element;
    }

    /**
     * @brief Generate a new list and append the specified list to the end of the list.
     *
     * @param list list to be appended to the list
     * @return the generated list
     */
    List operator+(const List& list) const
    {
        List new_list = *this;
        return new_list += list;
    }

    /**
     * @brief Generate a new list and remove the first occurrence of the specified element from the list, if it is present.
     *
     * @param element element to be removed
     * @return the generated list
     */
    List operator-(const T& element) const
    {
        List new_list = *this;
        return new_list -= element;
    }

    /**
     * @brief Generate a new list and add the list to itself a certain number of times.
     *
     * @param times times to repeat
     * @return the generated list
     */
    List operator*(int times) const
    {
        List new_list = *this;
        return new_list *= times;
    }
};

/*
 * Non-member functions
 */

/**
 * @brief Output list data to the specified output stream.
 *
 * @tparam T the type of elements in the list, must be printable
 * @param os an output stream
 * @param list the list to be printed to the output stream
 * @return self reference of the output stream
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const List<T>& list)
{
    if (list.is_empty())
    {
        return os << "[]";
    }

    auto it = list.begin();
    os << "[";
    while (true)
    {
        os << *it++;
        if (it == list.end())
        {
            return os << "]";
        }
        os << ", ";
    }
}

/**
 * @brief Generate a new list and add the list to itself a certain number of times.
 *
 * @tparam T the type of elements in the list
 * @param times times to repeat
 * @param list the list
 * @return the generated list
 */
template <typename T>
List<T> operator*(int times, const List<T>& list)
{
    return list * times;
}

} // namespace mdspp

#endif // LIST_HPP
