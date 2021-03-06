//
// Created by sascha on 6/2/17.
//

#ifndef MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
#define MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP

#include "matador/object/has_many_item_holder.hpp"

#include "matador/utils/cascade_type.hpp"

#include "matador/utils/access.hpp"

#include <iostream>

namespace matador {

template < class V, template < class ... > class C >
class has_many;

class object_holder;

template < class T, object_holder_type OHT >
class object_pointer;

namespace detail {

template < class Value, class Enable = void >
class relation_endpoint_value_inserter;

template < class Value >
class relation_endpoint_value_inserter<Value, typename std::enable_if<std::is_integral<int>::value>::type>
//class relation_endpoint_value_inserter<Value, typename std::enable_if<std::is_base_of<object_holder, Value>::value>::type>
{
public:
  template < class Owner >
  void insert(const object_pointer<Owner, object_holder_type::OBJECT_PTR> &owner, const std::string &field, has_many_item_holder<Value> holder);

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class T >
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) {}
  template < class T >
  void serialize(const char *, T &, cascade_type) {}
  void serialize(const char *id, object_pointer<Value, object_holder_type::BELONGS_TO> &x, cascade_type);
  void serialize(const char *id, object_pointer<Value, object_holder_type::HAS_ONE> &x, cascade_type);
  template < template < class ... > class Container >
  void serialize(const char *id, has_many<Value, Container> &x, const char*, const char*);
  template < class T, template < class ... > class Container >
  void serialize(const char *, has_many<T, Container> &, const char*, const char*) {}

private:
  std::string field_;
  has_many_item_holder<Value> holder_;
};

//template < class Value >
//class relation_endpoint_value_inserter<Value, typename std::enable_if<!std::is_base_of<object_holder, Value>::value>::type>
//{
//public:
//  template < class Owner >
//  void insert(const object_pointer<Owner, object_holder_type::OBJECT_PTR> &owner, const std::string &field, has_many_item_holder<Value> holder);
//
//  template < class T >
//  void serialize(T &x)
//  {
//    matador::access::serialize(*this, x);
//  }
//
//  template < class T >
//  void serialize(const char *, T &) {}
//  void serialize(const char *, char *, std::size_t ) {}
//  void serialize(const char *, object_holder &, cascade_type);
//  template < template < class ... > class Container >
//  void serialize(const char *, has_many<Value, Container> &, const char*, const char*);
//  template < class T, template < class ... > class Container >
//  void serialize(const char *, has_many<T, Container> &, const char*, const char*) {}
//
//private:
//  std::string field_;
//  has_many_item_holder<Value> holder_;
//};


}
}
#endif //MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
