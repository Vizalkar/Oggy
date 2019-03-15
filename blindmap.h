#ifndef BLINDMAP_H
#define BLINDMAP_H

#include <cassert>
#include <utility>
#include <unordered_map>
#include <vector>

namespace Oggy
{
/**
 * @brief Bread of std::vector, butter of std::unordered_map
 * Add an element, get it's unique ID
 * Access element with ID with unordered_map complexity
 * Browse elements with vector complexity
 */
template <typename ID, typename VAL>
class BlindMap
{
public:
    BlindMap() : m_nextID(){}
    ~BlindMap(){}

    /**
     * @brief addVal
     * @param val
     * @return id of the val
     * O(n) complexity
     */
    ID addVal(VAL val)
    {
        ID id(m_nextID);
        emplace(std::make_pair(id, val));
        m_refvec.push_back(id);
        incrementNextID();
        return id;
    }

    /**
     * @brief removeVal
     * @param id of the value to remove
     * O(n) complexity
     */
    void removeVal(ID id)
    {
        erase(id);
        for (auto it(m_refvec.begin()); it < m_refvec.end(); ++it){
            if (*it == id)
                m_refvec.erase(it);
        }
    }

    inline iterator find (const ID& id) { return m_umap.find(id); }
    inline const_iterator find (const ID& id) const { return m_umap.find(id); }

    inline iterator begin() const { return m_refvec.begin(); }
    inline iterator begin() const noexcept { return m_refvec.begin(); }
    inline const_iterator begin() const { return m_refvec.begin(); }
    inline const_iterator begin() const noexcept { return m_refvec.begin(); }
    inline const_iterator cbegin() const noexcept { return m_refvec.cbegin(); }

    inline iterator end() const { return m_refvec.end(); }
    inline iterator end() const noexcept { return m_refvec.end(); }
    inline const_iterator end() const { return m_refvec.end(); }
    inline const_iterator end() const noexcept { return m_refvec.end(); }
    inline const_iterator cend() const noexcept { return m_refvec.cend(); }

    inline iterator rbegin() const { return m_refvec.rbegin(); }
    inline iterator rbegin() const noexcept { return m_refvec.rbegin(); }
    inline const_iterator rbegin() const { return m_refvec.rbegin(); }
    inline const_iterator rbegin() const noexcept { return m_refvec.rbegin(); }
    inline const_iterator crbegin() const noexcept { return m_refvec.crbegin(); }

    inline iterator rend() const { return m_refvec.rend(); }
    inline iterator rend() const noexcept { return m_refvec.rend(); }
    inline const_iterator rend() const { return m_refvec.rend(); }
    inline const_iterator rend() const noexcept { return m_refvec.rend(); }
    inline const_iterator crend() const noexcept { return m_refvec.crend(); }

private:
    ID m_nextID;
    std::unordered_map<ID, VAL> m_umap;
    std::vector<ID> m_refvec;

    void incrementNextId()
    {
        ID ref(m_nextID), candidate(m_nextID);
        bool alreadyExists(false);
        while(++candidate != ref){
            for (const auto& id : m_refvec){
                if (id == candidate){
                    alreadyExists = true;
                    break;
                }
            }
            if (!alreadyExists){
                m_nextID = candidate;
                break;
            }
        }
    }
}

} // Oggy

#endif // BLINDMAP_H
