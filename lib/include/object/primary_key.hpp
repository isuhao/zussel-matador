#ifndef PRIMARY_KEY_HPP
#define PRIMARY_KEY_HPP

#include "object/object_atomizer.hpp"

#include "tools/enable_if.hpp"

#include <type_traits>

namespace oos {

class primary_key_base
{
public:
  virtual ~primary_key_base() {}

  friend bool operator==(primary_key_base const &a, primary_key_base const &b)
  {
    return a.equal_to(b);
  }
  friend bool operator<(primary_key_base const &a, primary_key_base const &b)
  {
    return a.less(b);
  }
  virtual void serialize(const char*, object_writer&) const = 0;
  virtual void deserialize(const char*, object_reader&) = 0;
  virtual primary_key_base* clone() const = 0;

protected:
  virtual bool equal_to(primary_key_base const &other) const = 0;
  virtual bool less(primary_key_base const &other) const = 0;
};

template < typename T, class Enable = void >
class primary_key;

// Todo: Implementation only for integer and string types
template < typename T >
class primary_key<T, typename oos::enable_if<std::is_integral<T>::value>::type > : public primary_key_base
{
public:
  typedef T value_type;

public:
  primary_key() : pk_(0) {}
  primary_key(T pk)
    : pk_(pk)
  {}

  primary_key& operator=(T pk)
  {
    pk_ = pk;
    return *this;
  }

  virtual ~primary_key() {}
  virtual void serialize(const char *id, object_writer &writer) const
  {
    writer.write(id, pk_);
  }

  virtual void deserialize(const char *id, object_reader &reader)
  {
    reader.read(id, pk_);
  }

  virtual primary_key_base* clone() const
  {
    return new primary_key<T>(pk_);
  }

  value_type get() const
  {
    return pk_;
  }

  operator value_type() const {
    return pk_;
  }

protected:

  virtual bool equal_to(primary_key_base const &other) const
  {
    if (primary_key<T> const *b = dynamic_cast<primary_key<T> const*>(&other)) {
      return pk_ == b->pk_;
    } else {
      return false;
    }
  }

  virtual bool less(primary_key_base const &other) const
  {
    if (primary_key<T> const *b = dynamic_cast<primary_key<T> const*>(&other)) {
      return pk_ < b->pk_;
    } else {
      return false;
    }
  }

private:
  T pk_;
};

}

/*
class obj
{

  oos::primary_key<long>::value_type id() { return id_; }
private:
  oos::primary_key<long> id_;
  oos::nullable<std::string> name_;
};
*/

#endif /* PRIMARY_KEY_HPP */
