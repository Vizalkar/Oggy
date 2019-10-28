#ifndef __OPAQUELIST__
#define __OPAQUELIST__

#include <utility>
#include <unordered_map>
#include <list>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <functional>
#include <cassert>

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
class OpaqueList
{
private:
    void incrementNextID();
public:
    OpaqueList() : m_nextID(1){}
    ~OpaqueList(){}

    /**
     * @return The ID value that represent a non bound object
     */
    static inline ID nullID()
    {
    	return ID(0);
    }

    /**
     * @brief addVal
     * @param val
     * @return id of the newly added value
     * O(n) complexity
     */
    ID addVal(VAL val)
    {
        ID id(m_nextID);
        incrementNextID();
		assert(emplaceVal(id, val));
        return id;
    }

	/**
	* @brief emplaceVal
	* @param id
	* @param val
	* @return true if val has effectively been emplaced, false if there were already a key in the bucket
	*/
	bool emplaceVal(ID id, VAL val)
	{
		m_valist.push_back(val);
		auto emplaceRet(m_umap.emplace(std::make_pair(id, std::ref(m_valist.back()))));
		m_ids.push_back(id);
		return emplaceRet.second;
	}

    /**
     * @brief removeVal
     * @param id of the value to remove
     * @return removed value
     * O(n) complexity
     */
    VAL removeVal(ID id)
    {
        const VAL* valptr = &(m_umap.at(id).get());
        for (auto it(m_ids.begin()); it != m_ids.end(); ++it){
            if (*it == id){
                m_ids.erase(it);
                m_umap.erase(id);
                break;
            }
        }
        for (auto it(m_valist.begin()); it != m_valist.end(); ++it){
            if (&(*it) == valptr){
                auto ret(*it);
                m_valist.erase(it);
                return ret;
            }
        }
        throw std::invalid_argument("given ID to remove does not exist !");
    }

    /**
     * @brief Moves value at position pos1 to position pos2 in the list. 
     * The list can then be iterated in the new order.
     */
    void moveVal(unsigned int pos1, unsigned int pos2)
    {
	    assert(pos1 < m_valist.size() && pos2 < m_valist.size());
	    auto it1(m_valist.begin());
	    auto it2(m_valist.begin());
	    unsigned int i(0);
	    for (; it1 != m_valist.end() && i < pos1; ++it1, ++i){}
	    i = 0;
	    for (; it2 != m_valist.end() && i < pos2; ++it2, ++i){}
	    if (pos1 < pos2) { m_valist.insert(++it2, *it1); }
		else { m_valist.insert(it2, *it1); }
	    m_valist.erase(it1);
    }

	unsigned int getPositionOfValue(ID seeked){
		unsigned int pos(0);
		for (const auto& id : m_ids){
			if (id == seeked) return pos;
			++pos;
		}
		assert(false && "Seeked ID does not exist");
		return 0;
	}
    
    using mconst_iterator = typename std::unordered_map<ID, VAL* const>::const_iterator;

    using viterator = typename std::list<VAL>::iterator;
    using vconst_iterator = typename std::list<VAL>::const_iterator;

    inline VAL& at (const ID& id) { return m_umap.at(id).get(); }
    inline const VAL& at (const ID& id) const { return m_umap.at(id).get(); }
    
    inline unsigned int count (const ID& id) const { return m_umap.count(id); }

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
    
    inline const std::vector<ID>& getIDVector() const { return m_ids; }
    inline unsigned int size() const { return m_ids.size(); }
    inline bool has(ID id) const { return m_umap.count(id); }

private:
    ID m_nextID;
    std::unordered_map<ID, std::reference_wrapper<VAL> const> m_umap;
    std::list<VAL> m_valist;
    std::vector<ID> m_ids;
};

template <typename ID, typename VAL>
void OpaqueList<ID,VAL>::incrementNextID()
{
    ID ref(m_nextID), candidate(m_nextID);
    ++candidate;
    while(candidate != ref && candidate != nullID()){
        if (m_umap.count(candidate) == 0){
            m_nextID = candidate;
            break;
        }
        ++candidate;
    }
}

} // Oggy

#endif //__OPAQUELIST__
