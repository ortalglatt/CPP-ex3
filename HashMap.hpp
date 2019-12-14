#ifndef EX3_HASHMAP_HPP
#define EX3_HASHMAP_HPP

#include <cassert>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <iostream>

/**
 * Defines the default capacity of a HashMap.
 */
const int DEF_CAPACITY = 16;
/**
 * Defines the minimum capacity of a HashMap.
 */
const int MIN_CAPACITY = 1;
/**
 * Defines the default size of a HashMap.
 */
const int DEF_SIZE = 0;
/**
 * Defines the default lower load factor of a HashMap.
 */
const double DEF_LOWER_FACTOR = 0.25;
/**
 * Defines the default upper load factor of a HashMap.
 */
const double DEF_UPPER_FACTOR = 0.75;
/**
 * Defines the resize factor.
 */
const int RESIZE_FACTOR = 2;
/**
 * Defines a massage for invalid load factors.
 */
const char* INVALID_FACTORS = "Invalid load factors";
/**
 * Defines a massage for two given vectors in different sizes.
 */
const char* INVALID_VECTORS = "Different size of vectors";
/**
 * Defines a massage for key that doesn't exist in the map.
 */
const char* INVALID_KEY = "Key doesn't exist in the map";

/**
 * Template class of HashMap.
 * @tparam KeyT The key object in the map.
 * @tparam ValueT The value object in the map.
 */
template <typename KeyT, typename ValueT>
class HashMap
{
    using pair = std::pair<KeyT, ValueT>;
    using bucket = std::vector<pair>;

    std::hash<KeyT> _hashFunc;
    int _capacity;
    int _curSize;
    double _lowerLoadFactor;
    double _upperLoadFactor;
    bucket *_map;

    /**
     * Calculate the hash code of the given key.
     * @param key The key to find the hash code.
     * @return The hash code of the key.
     */
    int _hashCode(const KeyT& key) const
    {
        return _hashFunc(key) & (_capacity - 1);
    }

    /**
     * Resize the map to the given new capacity, andrehash all the pairs that appeared in the old
     * map.
     * @param newCapacity the new capacity of the map.
     */
    void _resize(const int& newCapacity)
    {
        bucket *newMap = new bucket[newCapacity];
        int oldCapacity = _capacity;
        _capacity = newCapacity;
        for (int i = 0; i < oldCapacity; i++)
        {
            for (pair curPair: _map[i])
            {
                newMap[_hashCode(curPair.first)].push_back(curPair);
            }
        }
        std::swap(_map, newMap);
        delete[] newMap;
    }

    /**
     * Const iterator nested class.
     */
    class const_iterator
    {
    private:
        const HashMap *_hashMap;
        int _curBucket;
        int _cellInBucket;

    public:

        /**
         * The constructor. Check what is the first place after the given (bucket, cell) that is not
         * empty, and put it as the cell and bucket of the iterator.
         * @param hashMap A pointer to HashMap object.
         * @param bucket The current bucket to start the iterator from (default=0).
         * @param cell The current cell in the given bucket to start the iterator from (default=0).
         */
        explicit const_iterator(const HashMap *hashMap, int bucket = 0, int cell = 0) :
            _hashMap(hashMap),
            _curBucket(bucket),
            _cellInBucket(cell)
        {
            if (!_hashMap->empty())
            {
                while (_hashMap->_map[_curBucket].empty() && _curBucket < _hashMap->capacity())
                {
                    _curBucket++;
                }
            }
        }

        /**
         * Move the iterator to point on the next pair in the map.
         * @return The iterator after the change.
         */
        const_iterator& operator++()
        {
            _cellInBucket++;
            if (_cellInBucket == (int)_hashMap->_map[_curBucket].size())
            {
                while (_hashMap->_map[++_curBucket].empty())
                {
                    if (_curBucket == _hashMap->capacity())
                    {
                        break;
                    }
                }
                _cellInBucket = 0;
            }
            return *this;
        }

        /**
         * Move the iterator to point on the next pair in the map.
         * @return The iterator before the change.
         */
        const const_iterator operator++(int)
        {
            const_iterator temp = *this;
            ++(*this);
            return temp;
        }

        /**
         * Dereference on the iterator.
         * @return Reference to the pair the iterator points to.
         */
        const pair& operator*() const
        {
            return (_hashMap->_map[_curBucket][_cellInBucket]);
        }

        /**
         * @return Pointer to the pair the iterator points to.
         */
        const pair *operator->() const
        {
            return &(_hashMap->_map[_curBucket][_cellInBucket]);
        }

        /**
         * Compare between this iterator to the given one.
         * @param other iterator object to compare.
         * @return True if the two operators are equal, false otherwise.
         */
        bool operator==(const const_iterator& other) const
        {
            return (_hashMap == other._hashMap && _curBucket == other._curBucket &&
                    _cellInBucket == other._cellInBucket);
        }

        /**
         * Compare between this iterator to the given one.
         * @param other iterator object to compare.
         * @return True if the two operators are defferent, false otherwise.
         */
        bool operator!=(const const_iterator& other) const
        {
            return (!(*this == other));
        }

    };

public:

    /**
     * Constructor with two arguments. If the given factors are invalid, it will throw an exception.
     * @param lowerFactor The lower load factor of the map.
     * @param upperFactor the upper load factor of the map.
     */
    HashMap(double lowerFactor, double upperFactor) :
        _capacity(DEF_CAPACITY),
        _curSize(DEF_SIZE),
        _lowerLoadFactor(lowerFactor),
        _upperLoadFactor(upperFactor),
        _map(new bucket[_capacity])
        {
            if (_lowerLoadFactor <= 0 || _lowerLoadFactor >= 1 || _upperLoadFactor <= 0 ||
                _upperLoadFactor >= 1 || _lowerLoadFactor >= _upperLoadFactor)
            {
                delete[] _map;
                throw std::invalid_argument(INVALID_FACTORS);
            }
        }

    /**
     * Default constructor.
     */
    HashMap() :
        _capacity(DEF_CAPACITY),
        _curSize(DEF_SIZE),
        _lowerLoadFactor(DEF_LOWER_FACTOR),
        _upperLoadFactor(DEF_UPPER_FACTOR),
        _map(new bucket[_capacity]) {}


    /**
     * Constructor that gets two vectors (keys and values) and ut them into the map. If the vectors
     * are in different sizes, it will throw an exception.
     * @param keys
     * @param values
     */
    HashMap(const std::vector<KeyT>& keys, const std::vector<ValueT>& values) :
        _capacity(DEF_CAPACITY),
        _curSize(DEF_SIZE),
        _lowerLoadFactor(DEF_LOWER_FACTOR),
        _upperLoadFactor(DEF_UPPER_FACTOR),
        _map(new bucket[_capacity])
    {
        if (keys.size() != values.size())
        {
            delete[] _map;
            throw std::invalid_argument(INVALID_VECTORS);
        }
        try
        {
            for (int i = 0; i < keys.size(); i++)
            {
                (*this)[keys[i]] = values[i];
            }
        }
        catch (const std::bad_alloc&)
        {
            delete[] _map;
            throw;
        }
    }

    /**
     * Copy constructor.
     * @param other The HashMap object to copy.
     */
    HashMap(const HashMap& other) :
        _capacity(other._capacity),
        _curSize(other._curSize),
        _lowerLoadFactor(other._lowerLoadFactor),
        _upperLoadFactor(other._upperLoadFactor),
        _map(new bucket[_capacity])
    {
        for (int i = 0; i < _capacity; i ++)
        {
            bucket newBucket(other._map[i]);
            _map[i] = newBucket;
        }
    }

    /**
     * Move constructor.
     * @param other The HashMap object to copy.
     */
    HashMap(HashMap && other) noexcept :
        _capacity(other._capacity),
        _curSize(other._curSize),
        _lowerLoadFactor(other._lowerLoadFactor),
        _upperLoadFactor(other._upperLoadFactor),
        _map(other._map)
    {
        other._map = nullptr;
    }

    /**
     * HashMap destructor.
     */
    ~HashMap()
    {
        delete[] _map;
    }

    /**
     * @return The current size of the map, the number of elements it contains.
     */
    int size() const
    {
        return _curSize;
    }

    /**
     * @return The capacity of the map, the number of buckets in the map.
     */
    int capacity() const
    {
        return _capacity;
    }

    /**
     * @return The load factor of the map, the relation between the size and the capacity.
     */
    double getLoadFactor() const
    {
        return ((double) _curSize) / _capacity;
    }

    /**
     * @return True if the map is empty, false otherwise.
     */
    bool empty() const
    {
        return (_curSize == 0);
    }

    /**
     * If the key doesn't exist in the map, it will insert a pair of the given key and value to the
     * map.
     * @param key The key to insert.
     * @param value The value to insert.
     * @return True if the insert succeeded, false otherwise.
     */
    bool insert(const KeyT& key, const ValueT& value)
    {
        if (containsKey(key))
        {
            return false;
        }
        _curSize++;
        if (getLoadFactor() > _upperLoadFactor)
        {
            _resize(_capacity * RESIZE_FACTOR);
        }
        _map[_hashCode(key)].push_back(pair(key, value));
        return true;
    }

    /**
     * Check if the map contains the given key.
     * @param key the key to check.
     * @return true if the key is in the map, false otherwise.
     */
    bool containsKey(const KeyT& key) const
    {
        for (pair curPair : _map[_hashCode(key)])
        {
            if (curPair.first == key)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * throw exception if the key doesn't exist in the map.
     * @param key The key to check.
     * @return The value of the given key.
     */
    ValueT& at(const KeyT& key)
    {
        for (int i = 0; i < _map[_hashCode(key)].size(); i++)
        {
            if (_map[_hashCode(key)][i].first == key)
            {
                return _map[_hashCode(key)][i].second;
            }
        }
        throw std::invalid_argument(INVALID_KEY);
    }

    /**
     * throw exception if the key doesn't exist in the map.
     * @param key The key to check.
     * @return The value of the given key.
     */
    const ValueT& at(const KeyT& key) const
    {
        for (int i = 0; i < _map[_hashCode(key)].size(); i++)
        {
            if (_map[_hashCode(key)][i].first == key)
            {
                return _map[_hashCode(key)][i].second;
            }
        }
        throw std::invalid_argument(INVALID_KEY);
    }

    /**
     * If the key exist in the map, it will erase it.
     * @param key The key to erase.
     * @return True if the erase succeeded, false otherwise.
     */
    bool erase(const KeyT& key)
    {
        if (!containsKey(key))
        {
            return false;

        }
        auto vecIter = _map[_hashCode(key)].begin();
        while (vecIter->first != key)
        {
            vecIter++;
        }
        _curSize--;
        if (getLoadFactor() < _lowerLoadFactor && _capacity > MIN_CAPACITY)
        {
            _resize(_capacity / RESIZE_FACTOR);
        }
        _map[_hashCode(key)].erase(vecIter);
        return true;
    }

    /**
     * Throws an exception if the key doesn't exist in the map.
     * @param key The key to check the size of it's bucket.
     * @return The size of the bucket that contains the given key.
     */
    int bucketSize(const KeyT& key) const
    {
        if (containsKey(key))
        {
            return _map[_hashCode(key)].size();
        }
        throw std::invalid_argument(INVALID_KEY);
    }

    /**
     * Clear the map, erase all the pairs in it but doesn't change the other data members.
     */
    void clear()
    {
        for (int i = 0; i < _capacity; i ++)
        {
            _map[i].clear();
        }
        _curSize = 0;
    }

    /**
     * Copy assignment.
     * @param other HashMap object to copy.
     * @return This HashMap object, after the copy.
     */
    HashMap& operator=(const HashMap& other)
    {
        if (this != &other)
        {
            _capacity = other.capacity();
            _curSize = other.size();
            _lowerLoadFactor = other._lowerLoadFactor;
            _upperLoadFactor = other._upperLoadFactor;
            delete[] _map;
            _map = new bucket[_capacity];
            for (int i = 0; i < _capacity; i ++)
            {
                bucket newBucket(other._map[i]);
                _map[i] = newBucket;
            }
        }
        return *this;
    }

    /**
     * Move assignment.
     * @param other HashMap object to move.
     * @return This HashMap object, after the move.
     */
    HashMap& operator=(HashMap && other) noexcept
    {
        _capacity = other._capacity;
        _curSize = other._curSize;
        _lowerLoadFactor = other._lowerLoadFactor;
        _upperLoadFactor = other._upperLoadFactor;
        _map = other._map;
        other._map = nullptr;
        return *this;
    }

    /**
     * Undefined if the key doesn't exist in the map.
     * @param key The key to find it's value.
     * @return The value that in this key as a const reference.
     */
    const ValueT& operator[](const KeyT& key) const
    {
        try
        {
            return at(key);

        }
        catch (const std::invalid_argument&) {}
    }

    /**
     * If the key doesn't exist in the, it will create a pair with this key and a default value.
     * @param key The key to find it's value.
     * @return The value that in this key as a reference.
     */
    ValueT& operator[](const KeyT& key)
    {
        if (!containsKey(key))
        {
            ValueT value = 0;
            insert(key, value);
        }
        return at(key);
    }

    /**
     * @param other HashMap object to compare.
     * @return True if the capacity, size, factors and all the pairs in the map are equal, false
     * otherwise.
     */
    bool operator==(const HashMap& other) const
    {
        if (_curSize != other.size() || _capacity != other.capacity() ||
            _lowerLoadFactor != other._lowerLoadFactor ||
            _upperLoadFactor != other._upperLoadFactor)
        {
            return false;
        }
        for (int i = 0; i < _capacity; i++)
        {
            for (pair curPair : _map[i])
            {
                if (!other.containsKey(curPair.first) || curPair.second != other.at(curPair.first))
                {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @param other HashMap object to compare.
     * @return True if this and the given object are not equal, false otherwise.
     */
    bool operator!=(const HashMap& other) const
    {
        return (!(*this == other));
    }

    /**
     * @return An iterator object to the begin of the map.
     */
    const_iterator begin() const
    {
        return const_iterator(this);
    }

    /**
     * @return A const iterator object to the begin of the map.
     */
    const const_iterator cbegin() const
    {
        return const_iterator(this);
    }

    /**
     * @return An iterator object to the end of the map.
     */
    const_iterator end() const
    {
        return const_iterator(this, _capacity);
    }

    /**
     * @return A const iterator object to the end of the map.
     */
    const const_iterator cend() const
    {
        return const_iterator(this, _capacity);
    }

};

#endif //EX3_HASHMAP_HPP
