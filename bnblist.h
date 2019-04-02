#ifndef __BNBLIST__
#define __BNBLIST__

#include <utility>
#include <unordered_map>
#include <list>
#include <iterator>
#include <stdexcept>

namespace Oggy
{
/**
 * @brief Bread of std::list, butter of std::unordered_map
 * Add an element, get it's unique ID
 * Access element with ID with unordered_map complexity
 * Browse elements with list complexity
 * 
 * You cannot put more elements than the ID type allows (obviously)
 */
template <typename ID, typename VAL>
class BnBList
{
private:
    void incrementNextID();
public:
    BnBList() : m_nextID(){}
    ~BnBList(){}

    /**
     * @brief addVal
     * @param val
     * @return id of the val
     * O(n) complexity
     */
    ID addVal(VAL val)
    {
        ID id(m_nextID);
        m_valist.push_back(val);
        m_umap.emplace(std::make_pair(id, &m_valist.back()));
        incrementNextID();
        return id;
    }

    /**
     * @brief removeVal
     * @param id of the value to remove
     * @return has effectively removed
     * O(n) complexity
     */
    VAL removeVal(ID id)
    {
        const VAL* valptr = m_umap.find(id)->second;
        for (auto it(m_valist.begin()); it != m_valist.end(); ++it){
            if (&(*it) == valptr){
                m_valist.erase(it);
                return *it;
            }
        }
        throw std::invalid_argument("given ID to remove does not exist !");
    }
    
    using miterator = typename std::unordered_map<ID, const VAL*>::iterator;
    using mconst_iterator = typename std::unordered_map<ID, const VAL*>::const_iterator;

    using viterator = typename std::list<VAL>::iterator;
    using vconst_iterator = typename std::list<VAL>::const_iterator;

    inline miterator find (const ID& id) { return &m_umap.find(id); }
    inline mconst_iterator find (const ID& id) const { return &m_umap.find(id); }

    inline viterator begin() noexcept { return m_valist.begin(); }
    inline vconst_iterator begin() const noexcept { return m_valist.begin(); }
    inline vconst_iterator cbegin() const noexcept { return m_valist.cbegin(); }

    inline viterator end() noexcept { return m_valist.end(); }
    inline vconst_iterator end() const noexcept { return m_valist.end(); }
    inline vconst_iterator cend() const noexcept { return m_valist.cend(); }

    inline viterator rbegin() noexcept { return m_valist.rbegin(); }
    inline vconst_iterator rbegin() const noexcept { return m_valist.rbegin(); }
    inline vconst_iterator crbegin() const noexcept { return m_valist.crbegin(); }

    inline viterator rend() noexcept { return m_valist.rend(); }
    inline vconst_iterator rend() const noexcept { return m_valist.rend(); }
    inline vconst_iterator crend() const noexcept { return m_valist.crend(); }
    
    template< class Compare >
    inline void sort( Compare comp ) { m_valist.sort(comp); }
    
    inline void sort() { m_valist.sort(); }

private:
    ID m_nextID;
    std::unordered_map<ID, const VAL*> m_umap;
    std::list<VAL> m_valist;
};

template <typename ID, typename VAL>
void BnBList<ID,VAL>::incrementNextID()
{
    ID ref(m_nextID), candidate(m_nextID);
    while(++candidate != ref){
        if (m_umap.count(candidate) == 0){
            m_nextID = candidate;
            break;
        }
    }
}

} // Oggy

#endif //__BNBLIST__