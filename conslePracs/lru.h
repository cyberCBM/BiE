#pragma once
//LRU Cache
#include "stdafx.h"

template <typename K,    typename V>
class LRUCache
{
    // Key access history, most recent at back
    typedef std::list<K> List;

    // Key to value and key history iterator
    typedef unordered_map< K,
        std::pair<
        V,
        typename std::list<K>::iterator
        >
    > Cache;

    typedef V (*Fn)(const K&);

public:
    LRUCache(size_t aCapacity, Fn aFn)
        : mFn(aFn)
        , mCapacity(aCapacity)
    {}

    //get value for key aKey
    V operator()(const K& aKey)
    {
        typename Cache::iterator it = mCache.find(aKey);
        if (it == mCache.end()) //cache-miss: did not find the key
        {
            V v = mFn(aKey);
            insert(aKey, v);
            return v;
        }

        // cache-hit
        // Update access record by moving accessed key to back of the list
        mList.splice(mList.end(), mList, (it)->second.second);

        // return the retrieved value
        return (it)->second.first;
    }

private:
    // insert a new key-value pair in the cache
    void insert(const K& aKey, V aValue)
    {
        //method should be called only when cache-miss happens
        assert(mCache.find(aKey) == mCache.end());

        // make space if necessary
        if (mList.size() == mCapacity)
        {
            evict();
        }

        // record k as most-recently-used key
        typename std::list<K>::iterator it = mList.insert(mList.end(), aKey);

        // create key-value entry, linked to the usage record
        mCache.insert(std::make_pair(aKey, std::make_pair(aValue, it)));
    }

    //Purge the least-recently used element in the cache
    void evict()
    {
        assert(!mList.empty());

        // identify least-recently-used key
        const typename Cache::iterator it = mCache.find(mList.front());

        //erase both elements to completely purge record
        mCache.erase(it);
        mList.pop_front();
    }

private:
    List mList;
    Cache mCache;
    Fn mFn;
    size_t mCapacity;
};