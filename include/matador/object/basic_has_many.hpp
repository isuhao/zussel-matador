//
// Created by sascha on 1/6/16.
//

#ifndef OOS_BASIC_HAS_MANY_HPP
#define OOS_BASIC_HAS_MANY_HPP

#include "matador/object/has_one.hpp"
#include "matador/object/has_many_item.hpp"
#include "matador/object/abstract_has_many.hpp"
#include "matador/object/container_type_traits.hpp"

namespace matador {

/// @cond MATADOR_DEV

namespace detail {
class object_inserter;

template < class Value, class Enable >
class relation_endpoint_value_inserter;

template < class Value, class Enabled >
class relation_endpoint_value_remover;
}

template < class T, template <class ...> class C = std::vector >
class basic_has_many;

template < class T, template <class ...> class C = std::vector  >
class has_many;

template < class T, template <class ...> class C, class Enable = void >
struct has_many_iterator_traits;

template < class T, template <class ...> class C, class Enable = void >
struct const_has_many_iterator_traits;

template < class T, template <class ...> class C >
class has_many_iterator;

template < class T, template <class ...> class C >
class const_has_many_iterator;

/// @endcond

/**
 * @brief Base class for has many relationship classes
 *
 * This class acts as a base class for all relationship
 * classes. It provides all common functionality for these
 * classes.
 *
 * @tparam T The type of the contained objects
 * @tparam C The underlying container class
 */
template < class T, template <class ...> class C >
class basic_has_many : public abstract_has_many
{
public:
  typedef basic_has_many<T, C> base;                                    /**< Shortcut to self */
  typedef has_many_iterator<T, C> iterator;                             /**< Shortcut to iterator class */
  typedef has_many_iterator_traits<T, C> traits;                        /**< Shortcut to traits class */
  typedef typename iterator::value_type value_type;                     /**< Shortcut to value type */
  typedef typename traits::holder_container_type container_type;        /**< Shortcut to container type */
  typedef typename traits::holder_type holder_type;                     /**< Shortcut to container type */
  typedef typename traits::holder_container_type holder_container_type; /**< Shortcut to container type */
  typedef const_has_many_iterator<T,C> const_iterator;                  /**< Shortcut to const iterator */
  typedef typename container_type::size_type size_type;                 /**< Shortcut to size type */
  typedef typename container_type::iterator container_iterator;         /**< Shortcut to container iterator */

  typedef std::function<void(object_store&, object_proxy*)> mark_modified_owner_func; /**< Shortcut to mark modified owner function */

public:
  /**
   * @brief Returns the begin iterator of the container
   *
   * @return The begin iterator
   */
  iterator begin() { return iterator(holder_container_.begin()); }
  /**
   * @brief Returns the end iterator of the container
   *
   * @return The end iterator
   */
  iterator end() { return iterator(holder_container_.end()); }

  /**
   * @brief Returns the const begin iterator of the container
   *
   * @return The const begin iterator
   */
  const_iterator begin() const { return const_iterator(holder_container_.begin()); }
  /**
   * @brief Returns the const end iterator of the container
   *
   * @return The const end iterator
   */
  const_iterator end() const { return const_iterator(holder_container_.end()); }

  /**
   * @brief Returns a copy of the first element
   *
   * @return A copy of the first element
   */
  value_type front() { return *begin(); }

  /**
   * @brief Returns a const copy of the first element
   *
   * @return A const copy of the first element
   */
  const value_type front() const { return *begin(); }

  /**
   * @brief Returns a const copy of the first element
   *
   * @return A const copy of the first element
   */
  value_type back()
  {
    iterator tmp = end();
    --tmp;
    return *tmp;
  }

  /**
   * @brief Returns a const copy of the last element
   *
   * @return A const copy of the last element
   */
  const value_type back() const
  {
    const_iterator tmp = end();
    --tmp;
    return *tmp;
  }

  /**
   * @brief Returns the current size of the container
   *
   * @return The current size
   */
  size_type size() const { return holder_container_.size(); }

  /**
   * @brief Returns true if the container is empty
   *
   * @return True if the container is empty
   */
  bool empty() const { return holder_container_.empty(); }

  /**
   * @brief Clears the container
   */
  void reset() { holder_container_.clear(); }

  /**
   * @brief Appends an item to the end of the container
   *
   * @param item Element to be appended
   */
  void append(const typename iterator::holder_type &item) { holder_container_.push_back(item); }

  /// @cond MATADOR_DEV

protected:

  friend class detail::object_inserter;
  friend class object_store;
  friend class object_serializer;
  friend class detail::has_many_inserter<T, std::vector>;
  friend class detail::has_many_deleter<T, std::vector>;
  friend class detail::has_many_inserter<T, std::list>;
  friend class detail::has_many_deleter<T, std::list>;


  object_proxy *owner_ = nullptr;
  std::shared_ptr<basic_identifier> owner_id_;

  mark_modified_owner_func mark_modified_owner_;

  holder_container_type holder_container_;

  std::shared_ptr<detail::relation_endpoint<T>> relation_info_;
  /// @endcond

};

}
#endif //OOS_BASIC_HAS_MANY_HPP
