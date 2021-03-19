#include <cmath> 
#include <cstddef> 
#include <iostream> 
#include <list> 
#include <stdexcept> 
#include <utility> 
#include <vector> 
#include <algorithm>

template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap
{
public:
    class iterator
    {
    public:
        iterator();
        iterator(const iterator& other);
        iterator(typename std::list<std::pair<const KeyType, ValueType>>::iterator iter,
            typename std::vector<std::list<std::pair<const KeyType, ValueType>>>::iterator bucket_iter, HashMap* cont);

        iterator& operator=(const iterator& other);

        std::pair<const KeyType, ValueType>& operator*();
        typename std::list<std::pair<const KeyType, ValueType>>::iterator operator->();

        iterator& operator++();
        iterator operator++(int);

        bool operator==(iterator other) const;
        bool operator!=(iterator other) const;

    private:
        typename std::list<std::pair<const KeyType, ValueType>>::iterator iter;
        typename std::vector<std::list<std::pair<const KeyType, ValueType>>>::iterator bucket_iter;
        HashMap* cont;
    };

    class const_iterator
    {
    public:
        const_iterator();
        const_iterator(const const_iterator& other);
        const_iterator(iterator& other);
        const_iterator(typename std::list<std::pair<const KeyType, ValueType>>::const_iterator iter,
            typename std::vector<std::list<std::pair<const KeyType, ValueType>>>::const_iterator bucket_iter, const HashMap* cont);

        const std::pair<const KeyType, ValueType>& operator*() const;
        const typename std::list<std::pair<const KeyType, ValueType>>::const_iterator operator->() const;

        const_iterator& operator++();
        const_iterator operator++(int);

        bool operator==(const_iterator other) const;
        bool operator!=(const_iterator other) const;

    private:
        typename std::list<std::pair<const KeyType, ValueType>>::const_iterator iter;
        typename std::vector<std::list<std::pair<const KeyType, ValueType>>>::const_iterator bucket_iter;
        const HashMap* cont;
    };

    HashMap(Hash hasher = Hash());

    template <class Iterator>
    HashMap(Iterator begin, Iterator end, Hash hasher = Hash());
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init_list, Hash hasher = Hash());

    HashMap(HashMap& other);
    HashMap& operator=(HashMap& other);

    std::size_t size() const;
    bool empty() const;

    Hash hash_function() const;

    void insert(std::pair<const KeyType, ValueType> elem);
    void erase(KeyType key);

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator find(KeyType key) const;
    iterator find(KeyType key);

    ValueType& operator[](KeyType key);
    const ValueType& at(KeyType key) const;

    void clear();

private:
    std::vector<std::list<std::pair<const KeyType, ValueType>>> _data;
    std::vector<int> _used;

    std::size_t _size;
    std::size_t _capacity;

    Hash _hasher;

    void rehash(std::size_t new_cap);
};

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(Hash hasher)
    : _hasher(hasher)
    , _size(0)
{
    rehash(1);
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(HashMap& other)
    : _hasher(other._hasher)
{
    rehash(1);
    for (auto it = other.begin(); it != other.end(); ++it)
    {
        insert(*it);
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>& HashMap<KeyType, ValueType, Hash>::operator=(HashMap& other)
{
    if (this == &other)
    {
        return *this;
    }

    _hasher = other._hasher;
    clear();
    for (auto it = other.begin(); it != other.end(); ++it)
    {
        insert(*it);
    }

    return *this;
}

template<class KeyType, class ValueType, class Hash>
template<class Iterator>
HashMap<KeyType, ValueType, Hash>::HashMap(Iterator begin, Iterator end, Hash hasher)
    : _hasher(hasher)
    , _size(0)
{
    rehash(1);
    for (; begin != end; ++begin)
    {
        insert(*begin);
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init_list, Hash hasher)
    : _hasher(hasher)
    , _size(0)
{
    rehash(1);
    for (auto it = init_list.begin(); it != init_list.end(); ++it)
    {
        insert(*it);
    }
}

template<class KeyType, class ValueType, class Hash>
std::size_t HashMap<KeyType, ValueType, Hash>::size() const
{
    return _size;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::empty() const
{
    return _size == 0;
}

template<class KeyType, class ValueType, class Hash>
Hash HashMap<KeyType, ValueType, Hash>::hash_function() const
{
    return _hasher;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(std::pair<const KeyType, ValueType> elem)
{
    std::size_t hash = _hasher(elem.first) % _capacity;
    for (auto it = _data[hash].begin(); it != _data[hash].end(); ++it)
    {
        if (it->first == elem.first)
        {
            return;
        }
    }

    ++_size;
    _data[hash].push_back(std::pair<const KeyType, ValueType>(elem.first, elem.second));

    if (_capacity < _size)
        rehash(_size * 2);
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(KeyType key)
{
    std::size_t hash = _hasher(key) % _capacity;

    for (auto it = _data[hash].begin(); it != _data[hash].end(); ++it)
    {
        if (it->first == key)
        {
            _data[hash].erase(it);
            --_size;

            if (_size * 4 <= _capacity)
            {
                rehash(_size * 2);
            }
            if (empty())
            {
                rehash(1);
            }
            break;
        }
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::iterator::iterator()
    : cont(nullptr)
{}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::iterator::iterator(const iterator& other)
    : cont(other.cont)
    , bucket_iter(other.bucket_iter)
    , iter(other.iter)
{}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::iterator::iterator(typename std::list<std::pair<const KeyType, ValueType>>::iterator iter,
    typename std::vector<std::list<std::pair<const KeyType, ValueType>>>::iterator bucket_iter, HashMap* cont)
    : iter(iter)
    , bucket_iter(bucket_iter)
    , cont(cont)
{}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator& HashMap<KeyType, ValueType, Hash>::iterator::operator=(const iterator & other)
{
    cont = other.cont;
    bucket_iter = other.bucket_iter;
    iter = other.iter;

    return *this;
}

template<class KeyType, class ValueType, class Hash>
std::pair<const KeyType, ValueType>& HashMap<KeyType, ValueType, Hash>::iterator::operator*()
{
    return *iter;
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::iterator HashMap<KeyType, ValueType, Hash>::iterator::operator->()
{
    return iter;
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator& HashMap<KeyType, ValueType, Hash>::iterator::operator++()
{
    iter++;
    if (iter != bucket_iter->end())
    {
        return *this;
    }

    bucket_iter++;

    while (bucket_iter != cont->_data.end() && bucket_iter->empty())
    {
        iter = bucket_iter->begin();
        bucket_iter++;
    }

    if (bucket_iter != cont->_data.end())
    {
        iter = bucket_iter->begin();
    }

    return *this;
}


template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator HashMap<KeyType, ValueType, Hash>::iterator::operator++(int)
{
    iterator temp(*this);
    ++*this;

    return temp;
}


template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator& HashMap<KeyType, ValueType, Hash>::const_iterator::operator++()
{
    iter++;
    if (iter != bucket_iter->end())
    {
        return *this;
    }

    bucket_iter++;

    while (bucket_iter != cont->_data.end() && bucket_iter->size() == 0)
    {
        iter = bucket_iter->begin();
        bucket_iter++;
    }

    if (bucket_iter != cont->_data.end())
    {
        iter = bucket_iter->begin();
    }

    return *this;
}


template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator HashMap<KeyType, ValueType, Hash>::const_iterator::operator++(int)
{
    const_iterator temp(*this);
    ++* this;

    return temp;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::iterator::operator==(iterator other) const
{
    return cont == other.cont && bucket_iter == other.bucket_iter && iter == other.iter;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::iterator::operator!=(iterator other) const
{
    return !(*this == other);
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::const_iterator::const_iterator()
    : cont(nullptr)
{}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::const_iterator::const_iterator(const const_iterator& other)
    : cont(other.cont)
    , bucket_iter(other.bucket_iter)
    , iter(other.iter)
{}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::const_iterator::const_iterator(iterator & other)
    : cont(other.cont)
    , bucket_iter(other.bucket_iter)
    , iter(other.iter)
{}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::const_iterator::const_iterator(typename std::list<std::pair<const KeyType, ValueType>>::const_iterator iter,
    typename std::vector<std::list<std::pair<const KeyType, ValueType>>>::const_iterator bucket_iter, const HashMap* cont)
    : iter(iter)
    , bucket_iter(bucket_iter)
    , cont(cont)
{}

template<class KeyType, class ValueType, class Hash>
const std::pair<const KeyType, ValueType>& HashMap<KeyType, ValueType, Hash>::const_iterator::operator*() const
{
    return *iter;
}

template<class KeyType, class ValueType, class Hash>
const typename std::list<std::pair<const KeyType, ValueType>>::const_iterator HashMap<KeyType, ValueType, Hash>::const_iterator::operator->() const
{
    return iter;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::const_iterator::operator==(const_iterator other) const
{
    return cont == other.cont && bucket_iter == other.bucket_iter && iter == other.iter;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::const_iterator::operator!=(const_iterator other) const
{
    return !(*this == other);
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator HashMap<KeyType, ValueType, Hash>::begin()
{
    if (_size > 0)
    {
        auto it = _data.begin();
        while (it->empty())
            ++it;
        iterator res(it->begin(), it, this);
        return res;
    }
    else
    {
        return end();
    }
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator HashMap<KeyType, ValueType, Hash>::end()
{
    iterator res(_data[_capacity - 1].end(), _data.end(), this);
    return res;
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator HashMap<KeyType, ValueType, Hash>::begin() const
{
    if (_size > 0)
    {
        auto it = _data.begin();
        while (it->empty())
            ++it;
        const_iterator res(it->begin(), it, this);
        return res;
    }
    else
    {
        return end();
    }
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator HashMap<KeyType, ValueType, Hash>::end() const
{
    const_iterator res(_data[_capacity - 1].end(), _data.end(), this);
    return res;
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator HashMap<KeyType, ValueType, Hash>::find(KeyType key) const
{
    std::size_t hash = _hasher(key) % _capacity;

    for (auto it = _data[hash].begin(); it != _data[hash].end(); ++it)
    {
        if (it->first == key)
        {
            return HashMap::const_iterator(it, _data.begin() + hash, this);
        }
    }

    return end();
}

template<class KeyType, class ValueType, class Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator HashMap<KeyType, ValueType, Hash>::find(KeyType key)
{
    std::size_t hash = _hasher(key) % _capacity;

    for (auto it = _data[hash].begin(); it != _data[hash].end(); ++it)
    {
        if (it->first == key)
        {
            iterator res(it, _data.begin() + hash, this);
            return res;
        }
    }

    return end();
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[](KeyType key)
{
    auto it = find(key);
    if (it == end())
    {
        insert({ key, ValueType() });
        it = find(key);
    }

    return it->second;
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(KeyType key) const
{
    const_iterator it = find(key);
    if (it == end())
        throw std::out_of_range("out of range");
    return it->second;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear()
{
    _size = 0;
    _data.clear();
    rehash(1);
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::rehash(std::size_t new_cap)
{
    std::vector<std::list<std::pair<const KeyType, ValueType>>> data1 = _data;

    _size = 0;
    _data.clear();
    _data.resize(new_cap);

    _capacity = new_cap;

    for (auto it1 = data1.begin(); it1 != data1.end(); ++it1)
    {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
        {
            insert(*it2);
        }
    }
}