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
#include <algorithm>

#if defined(QT_CORE_LIB) && !defined(QT_NO_DEBUG)
    #include <QtGlobal>
    #define ASSERTMOD(test, str) { Q_ASSERT(test && str); }
#else
    #define ASSERTMOD(test, str) { test; }
#endif


namespace Oggy
{

template<typename I, typename UNDERLYING_TYPE>
class ID
{
public:
    friend struct std::hash<ID<I,UNDERLYING_TYPE>>;

    explicit ID(UNDERLYING_TYPE id = 0) : m_id(id) {}

    inline bool operator== (const ID<I,UNDERLYING_TYPE>& rhs) const {
        return m_id == rhs.m_id;
    }

    inline bool operator!=(const ID<I,UNDERLYING_TYPE>& rhs) const {
        return !(*this == rhs);
    }

    inline bool operator< (const ID<I,UNDERLYING_TYPE>& rhs) const {
        return m_id < rhs.m_id;
    }

    inline bool operator> (const ID<I,UNDERLYING_TYPE>& rhs) const {
        return rhs < *this;
    }

    inline bool operator<= (const ID<I,UNDERLYING_TYPE>& rhs) const {
        return !(*this > rhs);
    }

    inline bool operator>= (const ID<I,UNDERLYING_TYPE>& rhs) const {
        return !(*this < rhs);
    }

    inline explicit operator UNDERLYING_TYPE() const {
        return m_id;
    }

    inline UNDERLYING_TYPE getUnderlyingValue() const {
        return m_id;
    }

    inline ID<I,UNDERLYING_TYPE> operator++() {
        ++m_id;
        return *this;
    }

    inline ID<I,UNDERLYING_TYPE> operator--() {
        --m_id;
        return *this;
    }

    inline ID<I,UNDERLYING_TYPE> operator+( const ID<I,UNDERLYING_TYPE>& rhs) const {
        return ID<I,UNDERLYING_TYPE>(m_id + rhs.m_id);
    }

    inline ID<I,UNDERLYING_TYPE> operator-( const ID<I,UNDERLYING_TYPE>& rhs) const {
        return ID<I,UNDERLYING_TYPE>(m_id - rhs.m_id);
    }

    inline ID<I,UNDERLYING_TYPE> operator+( const UNDERLYING_TYPE& rhs) const {
        return ID<I,UNDERLYING_TYPE>(m_id + rhs);
    }

    inline ID<I,UNDERLYING_TYPE> operator-( const UNDERLYING_TYPE& rhs) const {
        return ID<I,UNDERLYING_TYPE>(m_id - rhs);
    }

    inline ID<I,UNDERLYING_TYPE> operator+=( const ID<I,UNDERLYING_TYPE>& rhs) {
        m_id += rhs.m_id;
        return *this;
    }

    inline ID<I,UNDERLYING_TYPE> operator-=( const ID<I,UNDERLYING_TYPE>& rhs) {
        m_id -= rhs.m_id;
        return *this;
    }

    inline bool isNull() const {
        return m_id == 0;
    }

    inline explicit operator bool() const{
        return !isNull();
    }

private:
    UNDERLYING_TYPE m_id;
};

} // OGGY

namespace std{
    template<typename I, typename UNDERLYING_TYPE>
    struct hash<Oggy::ID<I,UNDERLYING_TYPE>>;
}

namespace Oggy {

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
        ASSERTMOD(emplaceVal(id, val), "A key was replaced");
        return id;
    }

    ID addVal(VAL val, unsigned int pos)
    {
        auto id(addVal(val));
        moveVal(id, pos);
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
        auto emplaceRet(m_umap.emplace(std::make_pair(id, &m_valist.back())));
        m_ids.push_back(id);
        if (id == m_nextID) incrementNextID();
        ASSERTMOD(emplaceRet.second, "Emplace failed, a key with same value already exists in the bucket");
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
        ASSERTMOD(m_umap.count(id), "Has no such id in map");
        const VAL* valptr = m_umap.at(id);
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
        pos1 = std::min(pos1, static_cast<unsigned>(m_valist.size())-1u);
        pos2 = std::min(pos2, static_cast<unsigned>(m_valist.size())-1u);

        if (pos1 != pos2){
            auto it1(m_valist.begin());
            auto it2(m_valist.begin());
            std::advance(it1, pos1);
            std::advance(it2, pos2);

            auto itID1(m_ids.begin());
            std::advance(itID1, std::distance(m_valist.begin(), it1));

            auto itID2(m_ids.begin());
            std::advance(itID2, std::distance(m_valist.begin(), it2));

            if (pos1 < pos2) {
                it2 = m_valist.insert(++it2, *it1);
                itID2 = m_ids.insert(++itID2, *itID1);
            }
            else {
                it2 = m_valist.insert(it2, *it1);
                itID2 = m_ids.insert(itID2, *itID1);
            }

            it1 = (m_valist.begin());
            std::advance(it1, pos1 + (pos1>pos2 ? 1 : 0));

            itID1 = (m_ids.begin());
            std::advance(itID1, std::distance(m_valist.begin(), it1));

            auto id(*itID1);
            auto valptr(&(*it2));

            m_valist.erase(it1);
            m_ids.erase(itID1);

            m_umap.at(id) = valptr;
        }
    }

    inline void moveValFromID(ID id, unsigned int pos)
    {
        moveVal(getPositionOfValue(id), pos);
    }

    unsigned int getPositionOfValue(ID seeked)
    {
        unsigned int pos(0);
        for (const auto& id : m_ids){
            if (id == seeked) return pos;
            ++pos;
        }
        ASSERTMOD(false, "Seeked ID does not exist");
        return 0;
    }

    using mconst_iterator = typename std::unordered_map<ID, VAL* const>::const_iterator;

    using viterator = typename std::list<VAL>::iterator;
    using vriterator = typename std::list<VAL>::reverse_iterator;

    using vconst_iterator = typename std::list<VAL>::const_iterator;
    using vconst_riterator = typename std::list<VAL>::const_reverse_iterator;

    inline VAL& at (const ID& id) {
        ASSERTMOD(m_umap.count(id) != 0, "ID does not exist in map");
        return *m_umap.at(id);
    }
    
    inline const VAL& at (const ID& id) const {
        ASSERTMOD(m_umap.count(id) != 0, "ID does not exist in map");
        return *m_umap.at(id); 
    }

    inline unsigned int count (const ID& id) const { return m_umap.count(id); }
    inline bool empty() const { return m_valist.empty(); }

    inline viterator begin() noexcept { return m_valist.begin(); }
    inline vconst_iterator begin() const noexcept { return m_valist.begin(); }
    inline vconst_iterator cbegin() const noexcept { return m_valist.cbegin(); }

    inline viterator end() noexcept { return m_valist.end(); }
    inline vconst_iterator end() const noexcept { return m_valist.end(); }
    inline vconst_iterator cend() const noexcept { return m_valist.cend(); }

    inline vriterator rbegin() noexcept { return m_valist.rbegin(); }
    inline vconst_riterator rbegin() const noexcept { return m_valist.rbegin(); }
    inline vconst_riterator crbegin() const noexcept { return m_valist.crbegin(); }

    inline vriterator rend() noexcept { return m_valist.rend(); }
    inline vconst_riterator rend() const noexcept { return m_valist.rend(); }
    inline vconst_riterator crend() const noexcept { return m_valist.crend(); }

    inline void sort(const std::function<bool(const VAL& val1, const VAL& val2)>& compare) {
        m_valist.sort(compare);

        std::sort(m_ids.begin(), m_ids.end(), [this, compare](const ID& id1, const ID& id2){
            auto ptr1(m_umap.at(id1));
            auto it1(std::find_if(m_valist.begin(), m_valist.end(), [ptr1](const VAL& val){ return &val == ptr1; }));
            auto ptr2(m_umap.at(id2));
            auto it2(std::find_if(m_valist.begin(), m_valist.end(), [ptr2](const VAL& val){ return &val == ptr2; }));
            return compare(*it1, *it2);
        });
    }

    inline void sort() {
        sort([](const VAL& val1, const VAL& val2){
            return val1 < val2;
        });
    }

    inline const std::vector<ID>& getIDVector() const { return m_ids; }
    inline unsigned int size() const { return m_ids.size(); }
    inline bool has(ID id) const { return m_umap.count(id); }

private:
    ID m_nextID;
    std::unordered_map<ID, VAL*> m_umap;
    std::list<VAL> m_valist;
    std::vector<ID> m_ids;
};

template <typename ID, typename VAL>
void OpaqueList<ID,VAL>::incrementNextID()
{
    ID ref(m_nextID), candidate(m_nextID);
    ++candidate;
    while(candidate != ref){
        if (candidate != nullID() && m_umap.count(candidate) == 0){
            m_nextID = candidate;
            return;
        } else {
            ++candidate;   
        }
    }
    ASSERTMOD(false, "Has reached maximum number of objects in collection");
}

} // Oggy

namespace std{
    template<typename I, typename UNDERLYING_TYPE>
    struct hash<Oggy::ID<I, UNDERLYING_TYPE>>
    {
        std::size_t operator()(const Oggy::ID<I,UNDERLYING_TYPE>& id) const{
            return std::hash<UNDERLYING_TYPE>{}(id.m_id);
        }
    };
}

#endif //__OPAQUELIST__
