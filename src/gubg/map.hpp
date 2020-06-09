#ifndef HEADER_gubg_map_hpp_ALREADY_INCLUDED
#define HEADER_gubg_map_hpp_ALREADY_INCLUDED

namespace gubg { 

    template <typename Map, typename Key, typename Ftor>
    void with_value(const Map &map, const Key &key, Ftor &&ftor)
    {
        auto it = map.find(key);
        if (it != map.end())
            ftor(it->second);
    }

} 

#endif
