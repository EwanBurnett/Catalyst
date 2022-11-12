//TypeMap
//Maps types to values: Adapted from https://gpfault.net/posts/mapping-types-to-values.txt.html
//Ewan Burnett - 2022

//TODO: Comment This
#pragma once
#include <unordered_map>

namespace Containers {
    template<typename T>
    class TypeMap
    {
        using ValueType = T;
        using _Map = std::unordered_map<uint32_t, ValueType>;

    public:
        typedef typename _Map::iterator iterator;
        typedef typename _Map::const_iterator const_iterator;
        typedef typename _Map::value_type value_type;

        const_iterator begin() const { return m_Map.begin(); }
        const_iterator end() const { return m_Map.end(); }
        iterator begin() { return m_Map.begin(); }
        iterator end() { return m_Map.end(); }

        template<typename key>
        iterator find() { return m_Map.find(GetTypeID<key>()); }
        template<typename key>
        const_iterator find() const { return m_Map.find(GetTypeID<key>()); }

        template<typename key>
        void put(ValueType&& value)
        {
            m_Map[GetTypeID<key>()] = std::forward<ValueType>(value);
        }
    private:
        template<typename T>
        inline static uint32_t GetTypeID()
        {
            static const uint32_t id = m_LastID++;
            return id;
        }

        _Map m_Map;
        static uint32_t m_LastID;
    };

    template<typename ValueType>
    uint32_t TypeMap<ValueType>::m_LastID(0);
}

