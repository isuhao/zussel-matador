//
// Created by sascha on 12.03.16.
//

#ifndef OOS_ACTION_REMOVER_HPP
#define OOS_ACTION_REMOVER_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
#define MATADOR_OBJECT_API __declspec(dllexport)
#define EXPIMP_OBJECT_TEMPLATE
#else
#define MATADOR_OBJECT_API __declspec(dllimport)
#define EXPIMP_OBJECT_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define MATADOR_OBJECT_API
#endif

#include "matador/object/action_visitor.hpp"
#include "matador/object/action.hpp"

#include <vector>

namespace matador {

/// @cond MATADOR_DEV

class object_proxy;

class MATADOR_OBJECT_API action_remover : public action_visitor
{
public:
  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vactor;

public:
  action_remover(t_action_vactor &actions);
  virtual ~action_remover() {}

  bool remove(t_action_vactor::size_type index, object_proxy *proxy);

  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);

private:
  t_action_vactor &actions_;
  t_action_vactor::size_type index_;
  object_proxy *proxy_ = nullptr;
};

/// @endcond

}

#endif //OOS_ACTION_REMOVER_HPP
