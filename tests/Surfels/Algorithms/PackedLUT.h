#pragma once

#include <stdlib.h>
#include <utility>
#include <assert.h>
#include <stdexcept>
#include <limits>
#include <iostream>

namespace GISurfels {

    typedef uint64_t ID;
    static ID IDNotFound = 0;

    template<typename T>
    class PackedLookupTable
    {
    private:
        struct Allocation {
            ID objectID;
            uint64_t objectIndex;
            uint64_t nextAllocationIndex;
        };

        // Used to mark an allocation as owning no object
        static const uint64_t DeadObjectIndex = -1;

        // Used to mark an allocation as owning no object
        static const uint64_t NotAnIndex = -1;

        // Storage for objects
        // Objects are contiguous, and always packed to the start of the storage.
        // Objects can be relocated in this storage thanks to the separate list of allocations.
        size_t mObjectsCount = 0;
        uint64_t mCapacity = 0;
        uint64_t mMaxCapacity = std::numeric_limits<uint64_t>::max();
        T *mObjects = nullptr;

        // The allocation ID of each object in the object array (1-1 mapping)
        ID *mObjectIDs = nullptr;

        // FIFO queue to allocate objects
        Allocation *mAllocations = nullptr;

        // Last possible free allocation slot
        uint64_t mLastAllocationIndex = NotAnIndex;

        // The next index struct to use for an allocation
        uint64_t mNextAllocationIndex = NotAnIndex;

        Allocation *popFreeAllocation();

    public:
        struct Iterator {
        private:
            ID *mCurrentObjectID;

        public:
            Iterator(ID *in);

            Iterator &operator++();

            Iterator &operator+=(size_t offset);

            Iterator operator+(size_t offset);

            ID operator*() const;

            bool operator!=(const Iterator &other) const;
        };

        PackedLookupTable(size_t capacity);

        PackedLookupTable(const PackedLookupTable &other);

        PackedLookupTable(PackedLookupTable &&other);

        ~PackedLookupTable();

        void swap(PackedLookupTable &other);

        PackedLookupTable &operator=(PackedLookupTable rhs);

        T &operator[](ID id);

        const T &operator[](ID id) const;

        ID insert(const T &object);

        ID insert(T &&object);

        template<class... Args>
        ID emplace(Args &&... args);

        bool contains(ID id) const;

        void erase(ID id);

        void reserve(uint64_t capacity);

        bool empty() const;

        size_t size() const;

        size_t capacity() const;

        const T *data() const;

        Iterator begin() const;

        Iterator end() const;
    };

    template<typename T>
    typename PackedLookupTable<T>::Iterator begin(const PackedLookupTable<T> &fl) {
        return fl.begin();
    }

    template<typename T>
    typename PackedLookupTable<T>::Iterator end(const PackedLookupTable<T> &fl) {
        return fl.end();
    }

    template<typename T>
    void swap(PackedLookupTable<T> &lhs, PackedLookupTable<T> &rhs) {
        lhs.swap(rhs);
    }

    //--------------------------------------------------------------------------
    // Iterator

    template<typename T>
    PackedLookupTable<T>::Iterator::Iterator(ID* in)
        :
        mCurrentObjectID(in)
    {}

    template<typename T>
    typename PackedLookupTable<T>::Iterator&
        PackedLookupTable<T>::Iterator::operator++()
    {
        mCurrentObjectID++;
        return *this;
    }

    template<typename T>
    typename PackedLookupTable<T>::Iterator&
        PackedLookupTable<T>::Iterator::operator+=(size_t offset)
    {
        mCurrentObjectID += offset;
        return *this;
    }

    //--------------------------------------------------------------------------

    template<typename T>
    typename PackedLookupTable<T>::Iterator
        PackedLookupTable<T>::Iterator::operator+(size_t offset)
    {
        Iterator copy(*this);
        copy += offset;
        return copy;
    }

    template<typename T>
    ID
        PackedLookupTable<T>::Iterator::operator*() const
    {
        return *mCurrentObjectID;
    }

    template<typename T>
    bool
        PackedLookupTable<T>::Iterator::operator!=(const Iterator& other) const
    {
        return mCurrentObjectID != other.mCurrentObjectID;
    }

    //--------------------------------------------------------------------------
    template<typename T>
    typename PackedLookupTable<T>::Allocation*
        PackedLookupTable<T>::popFreeAllocation()
    {
        // Reserve more memory if needed
        if (mNextAllocationIndex == NotAnIndex) {
            reserve(mCapacity * 2);
        }

        assert(mObjectsCount < mCapacity);

        // pop an allocation from the FIFO
        Allocation* allocation = &mAllocations[mNextAllocationIndex];
        mNextAllocationIndex = allocation->nextAllocationIndex;

        // always allocate the object at the end of the storage
        allocation->objectIndex = mObjectsCount;
        mObjectsCount++;

        // update reverse-lookup so objects can know their ID
        mObjectIDs[allocation->objectIndex] = allocation->objectID;

        return allocation;
    }

    template<typename T>
    PackedLookupTable<T>::PackedLookupTable(size_t capacity)
    {
        mObjectsCount = 0;
        mCapacity = capacity;

        mObjects = reinterpret_cast<T*>(new char[capacity * sizeof(T)]);
        assert(mObjects);

        mObjectIDs = new ID[mCapacity];
        assert(mObjectIDs);

        mAllocations = new Allocation[mCapacity];
        assert(mAllocations);

        for (size_t i = 0; i < mCapacity; i++) {
            mAllocations[i].objectID = static_cast<ID>(i);
            mAllocations[i].objectIndex = DeadObjectIndex;
            mAllocations[i].nextAllocationIndex = static_cast<uint64_t>(i + 1);
        }

        if (mCapacity > 0) {
            mAllocations[mCapacity - 1].nextAllocationIndex = NotAnIndex;
        }

        mLastAllocationIndex = mCapacity - 1;
        mNextAllocationIndex = 0;
    }

    template<typename T>
    PackedLookupTable<T>::PackedLookupTable(const PackedLookupTable& other)
    {

        mObjectsCount = other.mObjectsCount;
        mCapacity = other.mCapacity;

        mObjects = reinterpret_cast<T*>(new char[other.mCapacity * sizeof(T)]);
        assert(mObjects);

        mObjectIDs = new ID[other.mCapacity];
        assert(mObjectIDs);

        mAllocations = new Allocation[other.mCapacity];
        assert(mAllocations);

        for (size_t i = 0; i < other.mCapacity; i++) {
            new(mObjects + i) T(*(other.mObjects + i));
            mObjectIDs[i] = other.mObjectIDs[i];
        }

        for (size_t i = 0; i < other.mCapacity; i++) {
            mAllocations[i] = other.mAllocations[i];
        }

        mLastAllocationIndex = other.mLastAllocationIndex;
        mNextAllocationIndex = other.mNextAllocationIndex;
    }

    template<typename T>
    PackedLookupTable<T>::PackedLookupTable(PackedLookupTable&& other)
    {
        swap(other);

        other.mObjects = nullptr;
        other.mObjectIDs = nullptr;
        other.mAllocations = nullptr;
    }

    template<typename T>
    PackedLookupTable<T>::~PackedLookupTable()
    {
        for (size_t i = 0; i < mObjectsCount; i++) {
            mObjects[i].~T();
        }
        delete[]((char*) mObjects);
        delete[] mObjectIDs;
        delete[] mAllocations;
    }

    template<typename T>
    void
        PackedLookupTable<T>::swap(PackedLookupTable& other)
    {
        std::swap(mObjectsCount, other.mObjectsCount);
        std::swap(mCapacity, other.mCapacity);
        std::swap(mObjects, other.mObjects);
        std::swap(mObjectIDs, other.mObjectIDs);
        std::swap(mAllocations, other.mAllocations);
        std::swap(mLastAllocationIndex, other.mLastAllocationIndex);
        std::swap(mNextAllocationIndex, other.mNextAllocationIndex);
    }

    template<typename T>
    PackedLookupTable <T>&
        PackedLookupTable<T>::operator=(PackedLookupTable rhs)
    {
        swap(rhs);
        return *this;
    }

    template<typename T>
    T&
        PackedLookupTable<T>::operator[](ID id)
    {
        // grab the allocation corresponding to this ID
        Allocation* allocation = &mAllocations[id];

        // grab the object associated with the allocation
        return *(mObjects + allocation->objectIndex);
    }

    template<typename T>
    const T&
        PackedLookupTable<T>::operator[](ID id) const
    {
        // grab the allocation corresponding to this ID
        Allocation* allocation = &mAllocations[id];

        // grab the object associated with the allocation
        return *(mObjects + allocation->objectIndex);
    }

    template<typename T>
    ID
        PackedLookupTable<T>::insert(const T& object)
    {
        Allocation* allocation = popFreeAllocation();
        T* insertionPointer = mObjects + allocation->objectIndex;
        new(insertionPointer) T(object);
        return allocation->objectID;
    }

    template<typename T>
    ID
        PackedLookupTable<T>::insert(T&& object)
    {
        Allocation* allocation = popFreeAllocation();
        T* insertionPointer = mObjects + allocation->objectIndex;
        new(insertionPointer) T(std::move(object));
        return allocation->objectID;
    }

    template<typename T>
    template<class... Args>
    ID
        PackedLookupTable<T>::emplace(Args &&... args)
    {
        Allocation* allocation = popFreeAllocation();
        T* o = mObjects + allocation->objectIndex;
        new(o) T(std::forward<Args>(args)...);
        return allocation->objectID;
    }

    template<typename T>
    bool
        PackedLookupTable<T>::contains(ID id) const
    {
        Allocation* allocation = &mAllocations[id];
        bool contains = allocation->objectID == id && allocation->objectIndex != DeadObjectIndex;
        return contains;
    }

    template<typename T>
    void
        PackedLookupTable<T>::erase(ID id)
    {
        if (!contains(id)) {
            throw std::invalid_argument(string_format("There is no object with ID %d\n", id));
        }

        // grab the allocation to delete
        ID eresableAllocationIndex = id;
        Allocation* allocation = &mAllocations[eresableAllocationIndex];

        // grab the object for this allocation
        T* object = mObjects + allocation->objectIndex;

        // if necessary, move (aka swap) the last object into the location of the object to erase, then unconditionally delete the last object
        if (allocation->objectIndex != mObjectsCount - 1) {
            T* last = mObjects + mObjectsCount - 1;
            *object = std::move(*last);
            object = last;

            // since the last object was moved into the deleted location, the associated object ID array's value must also be moved similarly
            ID lastObjectID = mObjectIDs[mObjectsCount - 1];
            mObjectIDs[allocation->objectIndex] = lastObjectID;

            // since the last object has changed location, its allocation needs to be updated to the new location.
            uint64_t lastAllocationIndex = lastObjectID;
            mAllocations[lastAllocationIndex].objectIndex = allocation->objectIndex;
        }

        // destroy the removed object and pop it from the array
        object->~T();
        mObjectsCount--;

        uint64_t deletedAllocationIndex = allocation->objectID;

        if (mNextAllocationIndex == NotAnIndex) {
            mNextAllocationIndex = deletedAllocationIndex;
        }

        // push the deleted allocation onto the FIFO
        mAllocations[mLastAllocationIndex].nextAllocationIndex = deletedAllocationIndex;
        mLastAllocationIndex = deletedAllocationIndex;

        // put a tombstone where the allocation used to point to an object index
        allocation->objectIndex = DeadObjectIndex;
        allocation->nextAllocationIndex = NotAnIndex;
    }

    template<typename T>
    void
        PackedLookupTable<T>::reserve(uint64_t capacity)
    {
        if (mCapacity == mMaxCapacity) {
            //throw std::length_error((std::string("Cannot reserve more memory. Maximum capacity (%d) has already been reached\n"), mMaxCapacity))
                std::cout << "[Packed LUT] maximum capacity has reached : " << mMaxCapacity << std::endl;
        }

        uint64_t newCapacity = std::min(capacity, mMaxCapacity);

        T* objects = reinterpret_cast<T*>(new char[newCapacity * sizeof(T)]);
        ID* objectIDs = new ID[newCapacity];
        Allocation* allocations = new Allocation[newCapacity];

        memcpy((char*) objects, (char*) mObjects, mObjectsCount * sizeof(T));
        memcpy(allocations, mAllocations, mCapacity * sizeof(Allocation));
        memcpy(objectIDs, mObjectIDs, mCapacity * sizeof(ID));

        for (size_t i = mCapacity; i < newCapacity; ++i) {
            allocations[i].objectID = static_cast<ID>(i);
            allocations[i].objectIndex = DeadObjectIndex;
            allocations[i].nextAllocationIndex = static_cast<uint64_t>(i + 1);
        }

        // Link last allocation to the new memory chunk
        allocations[mLastAllocationIndex].nextAllocationIndex = mObjectsCount;
        allocations[newCapacity - 1].nextAllocationIndex = NotAnIndex;
        mNextAllocationIndex = mObjectsCount;
        mLastAllocationIndex = newCapacity - 1;

        delete[](char*) mObjects;
        delete[] mObjectIDs;
        delete[] mAllocations;

        mObjects = objects;
        mObjectIDs = objectIDs;
        mAllocations = allocations;

        mCapacity = newCapacity;
    }

    template<typename T>
    bool
        PackedLookupTable<T>::empty() const
    {
        return mObjectsCount == 0;
    }

    template<typename T>
    size_t
        PackedLookupTable<T>::size() const
    {
        return mObjectsCount;
    }

    template<typename T>
    size_t
        PackedLookupTable<T>::capacity() const
    {
        return mCapacity;
    }

    template<typename T>
    const T*
        PackedLookupTable<T>::data() const
    {
        return mObjects;
    }

    template<typename T>
    typename PackedLookupTable<T>::Iterator
        PackedLookupTable<T>::begin() const
    {
        return Iterator{ mObjectIDs };
    }

    template<typename T>
    typename PackedLookupTable<T>::Iterator
        PackedLookupTable<T>::end() const
    {
        return Iterator{ mObjectIDs + mObjectsCount };
    }
}
