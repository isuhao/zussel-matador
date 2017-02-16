//
// Created by sascha on 12/10/15.
//

#ifndef OBJECT_HOLDER_HPP
#define OBJECT_HOLDER_HPP

#ifdef _MSC_VER
#ifdef oos_object_EXPORTS
    #define OOS_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define OOS_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4275)
#else
#define OOS_OBJECT_API
#endif

#include "oos/utils/cascade_type.hpp"
#include "oos/utils/identifiable_holder.hpp"

#include "oos/object/object_holder_type.hpp"

#include <memory>

namespace oos {

namespace detail {
class object_inserter;
class object_deleter;
class object_proxy_accessor;
template<class T, template <class ...> class C, class Enabled>
class has_many_inserter;
template<class T, template <class ...> class C, class Enabled>
class has_many_deleter;
}

class basic_identifier;
class object_proxy;
class object_store;

/**
 * @class object_holder
 * @brief Base class for the object pointer and reference class
 *
 * This is the base class for the object pointer
 * and reference class. The class holds the proxy
 * of the object and the id of the object.
 */
class OOS_OBJECT_API object_holder : public identifiable_holder
{
protected:
  /**
   * @brief Creates and empty base pointer.
   * 
   * Creates and empty base pointer. The boolean
   * tells the class if the object is handled
   * as a reference or an pointer. The difference
   * is that the reference couldn't be deleted
   * from the object_store and the pointer can.
   * 
   * @param is_internal True if the pointer is used internal, which means
   *                    it is used to describe an entity.
   */
  explicit object_holder(object_holder_type holder_type);

  /**
   * Copies from another object_holder
   * 
   * @param x the object_holder to copy from.
   */
  object_holder(const object_holder &x);

  /**
   * Assign operator.
   * 
   * @param x The object_holder to assign from.
   */
  object_holder & operator=(const object_holder &x);

  /**
   * @brief Creates an object_holder with a given object_proxy
   * 
   * Creates an object_holder with a given object_proxy. The
   * boolean tells the object_holder if it should be
   * handled as an internal.
   * 
   * @param is_internal If true the object is handled as an internal.
   * @param op The object_proxy of the object_holder
   */
  object_holder(object_holder_type holder_type, object_proxy *op);

  /**
   * Destroys the object_holder
   * and decides wether the underlaying
   * object_proxy is destroyed as well.
   *
   * It is destroyed if it is not inserted
   * into any object_store.
   */
  virtual ~object_holder();

public:

  /**
   * Equal to operator for the object_holder
   * 
   * @param x The object_holder to check equality with.
   */
  bool operator==(const object_holder &x) const;

  /**
   * Not equal to operator for the object_holder
   * 
   * @param x The object_holder to check unequality with.
   */
  bool operator!=(const object_holder &x) const;

  /**
   * Resets the object_holder with the given object_proxy.
   * 
   * @param proxy The new object_proxy for the object_holder.
   * @param cascade Sets the cascadable actions for the proxy.
   */
  void reset(object_proxy *proxy, cascade_type cascade);

  /**
   * Resets the object_holder with the given
   * identifier. If the type of identifier differs
   * from internal type an exception is thrown
   *
   * @param id The identifier to set
   */
  void reset(const std::shared_ptr<basic_identifier> &id);

  /**
   * Clears the currently set object
   */
  void clear();

  /**
   * Returns true if object_holder doesn't
   * holds an object
   *
   * @return True if object_holder doesn't holds an object
   */
  bool empty() const;

  /**
   * Returns if the object is loaded.
   * 
   * @return True if the object is loaded.
   */
  bool is_loaded() const;

  /**
   * Returns the object id.
   * 
   * @return The id of the object.
   */
  unsigned long id() const;

  /**
   * Sets the object id. If a proxy
   * is set an exception is thrown.
   * 
   * @param i The new object id
   */
  void id(unsigned long i);

  /**
   * Returns the corresponding
   * object_store or nullptr
   */
  object_store* store() const;

  /**
   * Returns the raw object pointer
   * 
   * @return The raw object pointer.
   */
  void* ptr();

  /**
   * Returns the raw object pointer
   *
   * @return The raw object pointer.
   */
  const void* ptr() const;

  /**
   * Returns the object pointer
   * 
   * @return The object pointer.
   */
  void* lookup_object();

  /**
   * Returns the object pointer
   *
   * @return The object pointer.
   */
  void* lookup_object() const;

  /**
   * Returns if this object_holder is inside
   * of the object_store. This is important
   * to calculate the reference and pointer
   * counter.
   * 
   * @return True if the object_holder internal
   */
  bool is_internal() const;

  bool is_belongs_to() const;

  bool is_has_one() const;

  bool is_object_ptr() const;
  /**
   * Returns true if the underlying object
   * is inserted in an object_store
   *
   * @return True if object is inserted.
   */
  bool is_inserted() const;

  /**
   * Returns true if object has a primary key
   *
   * @return true if object has a primary key
   */
  bool has_primary_key() const;

  /**
   * Gets the primary key of the foreign object
   *
   * @return The primary key of the foreign object
   */
  virtual std::shared_ptr<basic_identifier> primary_key() const;

  /**
   * Returns the current reference count
   *
   * @return The current reference count
   */
  unsigned long reference_count() const;

  /**
   * Return the type string of the object
   *
   * @return The type string of the object.
   */
  virtual const char* type() const = 0;

  object_holder_type holder_type() const;

  /**
   * Prints the underlaying object
   *
   * @param out The output stream to write on.
   * @param x The object pointer to print.
   * @return The output stream.
   */
  friend OOS_OBJECT_API std::ostream& operator<<(std::ostream &out, const object_holder &x);

private:
  friend class object_serializer;
  friend class object_proxy;
  friend class detail::object_deleter;
  friend class detail::object_inserter;
  friend class object_store;
  friend class object_container;
  friend class detail::object_proxy_accessor;
  template<class T, template <class ...> class C, class Enabled >
  friend class detail::has_many_inserter;
  template<class T, template <class ...> class C, class Enabled >
  friend class detail::has_many_deleter;

  // Todo: change interface to remove friend
  friend class session;
  // Todo: replace private access of proxy with call to reset
  friend class table_reader;

  template < class T, object_holder_type OPT > friend class object_pointer;

  object_proxy *proxy_ = nullptr;
  object_proxy *owner_ = nullptr; // only set if holder type is BELONGS_TO or HAS_MANY
  cascade_type cascade_ = cascade_type::NONE;
  object_holder_type type_;
  bool is_inserted_ = false;
  unsigned long oid_ = 0;
};

}

#endif //OBJECT_HOLDER_HPP
