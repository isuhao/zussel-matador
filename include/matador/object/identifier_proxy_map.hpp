/**
 * @author sascha on 5/2/16.
 */

#ifndef OOS_IDENTIFIER_PROXY_MAP_HPP
#define OOS_IDENTIFIER_PROXY_MAP_HPP

#include "matador/utils/basic_identifier.hpp"

#include "matador/object/basic_has_many_item_holder.hpp"

#include <unordered_map>

namespace matador {
class object_proxy;

namespace detail {

/// @cond MATADOR_DEV

template<class T> class identifier_hash;

template<>
class identifier_hash<std::shared_ptr<basic_identifier> >
{
public:
  size_t operator()(const std::shared_ptr<basic_identifier> &id) const
  {
    return id->hash();
  }
};

class identifier_equal
{
public:
  bool operator()(const std::shared_ptr<basic_identifier>& lhs,
                  const std::shared_ptr<basic_identifier>& rhs) const
  {
    return lhs->is_same_type(*rhs) && lhs->equal_to(*rhs);
  }
};

typedef std::shared_ptr<basic_identifier> identifier_ptr; /**< Shortcut to shared identifier ptr */
typedef std::unordered_map<identifier_ptr, object_proxy*, identifier_hash<identifier_ptr>, identifier_equal> t_identifier_map;
typedef std::unordered_multimap<identifier_ptr, std::shared_ptr<basic_has_many_item_holder>, identifier_hash<identifier_ptr>, identifier_equal> t_identifier_multimap;

/// @endcond

}
}

#endif //OOS_IDENTIFIER_PROXY_MAP_HPP
