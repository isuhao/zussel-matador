//
// Created by sascha on 6/9/17.
//

#include "matador/object/object_store.hpp"

#include "../Item.hpp"

#include "RelationTestUnit.hpp"

RelationTestUnit::RelationTestUnit()
  : unit_test("relation", "Relation Test Unit")
{
  add_test("has_one", std::bind(&RelationTestUnit::test_has_one, this), "test has one relation");
  add_test("belongs_to_one", std::bind(&RelationTestUnit::test_belongs_to_one, this), "test belongs to one relation");
  add_test("has_many_vector", std::bind(&RelationTestUnit::test_has_many_vector, this), "test has many vector relation");
  add_test("has_many_list", std::bind(&RelationTestUnit::test_has_many_list, this), "test has many list relation");
  add_test("has_many_builtin", std::bind(&RelationTestUnit::test_has_many_builtin, this), "test has many relation with builtin");
  add_test("belongs_to_many", std::bind(&RelationTestUnit::test_belongs_to_many, this), "test belongs to many relation");
  add_test("has_many_to_many", std::bind(&RelationTestUnit::test_has_many_to_many, this), "test has many to many relation");
  add_test("has_many_to_many_remove", std::bind(&RelationTestUnit::test_has_many_to_many_remove, this), "test has many to many remove relation");
}

void RelationTestUnit::test_has_one()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<master>("master");

  UNIT_ASSERT_EQUAL(2UL, store.size(), "must be two nodes");

  auto node = store.find("master");

  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "child", "endpoint field name must be 'child'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HS_ONE");

  node = store.find<child>();

  UNIT_ASSERT_TRUE(node->endpoints_empty(), "endpoints must be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 0UL, "endpoints must be zero");

  auto chld = store.insert(new child("child 1"));

  UNIT_ASSERT_TRUE(chld.id() > 0UL, "id must be greater zero");

  auto mstr = store.insert(new master("m1", chld));

  UNIT_ASSERT_TRUE(mstr.id() > 0UL, "id must be greater zero");

  UNIT_ASSERT_NOT_NULL(mstr->children.get(), "id must not be null");

  mstr->children = nullptr;

  UNIT_ASSERT_NULL(mstr->children.get(), "id must be null");
}

void RelationTestUnit::test_belongs_to_one()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("citizen");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "address", "endpoint field name must be 'address'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  node = store.find<address>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "citizen", "endpoint field name must be 'citizen'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  auto george = store.insert(new citizen("george"));
  auto home = store.insert(new address("foreststreet", "foresting"));

  UNIT_ASSERT_TRUE(george.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_TRUE(home.id() > 0UL, "id must be greater zero");
  UNIT_ASSERT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_NULL(home->citizen_.get(), "id must be null");

  george->address_ = home;

  UNIT_ASSERT_NOT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_NOT_NULL(home->citizen_.get(), "id must be null");

  george->address_ = nullptr;

  UNIT_ASSERT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_NULL(home->citizen_.get(), "id must be null");

  home->citizen_ = george;

  UNIT_ASSERT_NOT_NULL(george->address_.get(), "id must be null");
  UNIT_ASSERT_NOT_NULL(home->citizen_.get(), "id must be null");
}

void RelationTestUnit::test_has_many_vector()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_vector>("children_vector");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("children_vector");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children", "endpoint field name must be 'children'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

//  auto foreign_endpoint = endpoint->second->foreign_endpoint.lock();
//
//  UNIT_ASSERT_NOT_NULL(foreign_endpoint.get(), "foreign endpoint must be valid");
//  UNIT_ASSERT_EQUAL(foreign_endpoint->field, "vector_id", "endpoint field name must be 'vector_id'");
//  UNIT_ASSERT_EQUAL(foreign_endpoint->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL, "endpoints must be one");

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id", "endpoint field name must be 'child_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  endpoint = node->find_endpoint("vector_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "vector_id", "endpoint field name must be 'vector_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  auto tim = store.insert(new child("tim"));
  auto group = store.insert(new children_vector("group"));

  UNIT_ASSERT_TRUE(group->children.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL, "vector size must be zero");

  group->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL, "vector size must be one");

  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim", "name must be 'tim'");
}

void RelationTestUnit::test_has_many_list()
{
  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_list>("children_list");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("children_list");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "children", "endpoint field name must be 'children'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

//  auto foreign_endpoint = endpoint->second->foreign_endpoint.lock();
//
//  UNIT_ASSERT_NOT_NULL(foreign_endpoint.get(), "foreign endpoint must be valid");
//  UNIT_ASSERT_EQUAL(foreign_endpoint->field, "list_id", "endpoint field name must be 'list_id'");
//  UNIT_ASSERT_EQUAL(foreign_endpoint->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find("children");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL, "endpoints must be one");

  endpoint = node->find_endpoint("child_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "child_id", "endpoint field name must be 'child_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  endpoint = node->find_endpoint("list_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "list_id", "endpoint field name must be 'list_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  auto tim = store.insert(new child("tim"));
  auto group = store.insert(new children_list("group"));

  UNIT_ASSERT_TRUE(group->children.empty(), "list must be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 0UL, "list size must be zero");

  group->children.push_back(tim);

  UNIT_ASSERT_FALSE(group->children.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(group->children.size(), 1UL, "vector size must be one");

  UNIT_ASSERT_EQUAL(group->children.front()->name, "tim", "name must be 'tim'");
}

void RelationTestUnit::test_has_many_builtin()
{
  using many_vector_ints = many_builtins<int, std::vector>;
//  using many_vector_strings = many_builtins<std::string, std::vector>;

  matador::object_store store;

  store.attach<many_vector_ints>("many_vector_ints");

  UNIT_ASSERT_EQUAL(2UL, store.size(), "must be two nodes");

  auto node = store.find("many_vector_ints");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->find_endpoint("elements");

  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "elements", "endpoint field name must be 'elements'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  node = store.find("elements");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->find_endpoint("list_id");

  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "must find endpoint");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "list_id", "endpoint field name must be 'list_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_ONE, "endpoint type must be HAS_ONE");

  endpoint = node->find_endpoint("value");

  UNIT_ASSERT_TRUE(endpoint == node->endpoint_end(), "must not find endpoint 'value'");

  auto many_ints = store.insert(new many_vector_ints);

  UNIT_ASSERT_TRUE(many_ints->elements.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(many_ints->elements.size(), 0UL, "vector size must be zero");

  many_ints->elements.push_back(7);

  UNIT_ASSERT_FALSE(many_ints->elements.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(many_ints->elements.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(many_ints->elements.front(), 7, "values must be equal");
}

void RelationTestUnit::test_belongs_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<department>("department");
  store.attach<employee, person>("employee");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  auto node = store.find("employee");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "department", "endpoint field name must be 'department'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find<department>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "employee", "endpoint field name must be 'employee'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  auto jane = store.insert(new employee("jane"));
  auto insurance = store.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");

  jane->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");

  jane->dep(nullptr);

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");

  insurance->employees.push_back(jane);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");

  insurance->employees.clear();

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");

  jane->dep(insurance);

  UNIT_ASSERT_FALSE(insurance->employees.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->dep(), insurance, "objects must be equal");

  insurance->employees.remove(jane);

  UNIT_ASSERT_TRUE(insurance->employees.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(insurance->employees.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_NULL(jane->dep().get(), "object must be null");
}

void RelationTestUnit::test_has_many_to_many()
{
  matador::object_store store;

  store.attach<person>("person");
  store.attach<student, person>("student");
  store.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, store.size(), "must be four nodes");

  auto node = store.find("student");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  auto endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_course", "endpoint field name must be 'student_course'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  node = store.find("student_course");
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 2UL, "endpoints must be one");

  endpoint = node->find_endpoint("course_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "endpoint must be found");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "course_id", "endpoint field name must be 'course_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  endpoint = node->find_endpoint("student_id");
  UNIT_ASSERT_FALSE(endpoint == node->endpoint_end(), "endpoint must be found");
  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_id", "endpoint field name must be 'student_id'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::BELONGS_TO, "endpoint type must be BELONGS_TO");

  node = store.find<course>();
  UNIT_ASSERT_TRUE(node != store.end(), "must find a node");
  UNIT_ASSERT_FALSE(node->endpoints_empty(), "endpoints must not be empty");
  UNIT_ASSERT_EQUAL(node->endpoints_size(), 1UL, "endpoints must be one");

  endpoint = node->endpoint_begin();

  UNIT_ASSERT_EQUAL(endpoint->second->field, "student_course", "endpoint field name must be 'student_course'");
  UNIT_ASSERT_EQUAL(endpoint->second->type, matador::detail::basic_relation_endpoint::HAS_MANY, "endpoint type must be HAS_MANY");

  auto jane = store.insert(new student("jane"));
  auto tom = store.insert(new student("tom"));
  auto art = store.insert(new course("art"));

  UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");

  jane->courses.push_back(art); // jane (value) must be push_back to course art (owner) students!!

  UNIT_ASSERT_FALSE(jane->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->courses.front(), art, "objects must be same");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.front(), jane, "objects must be same");

  art->students.push_back(tom);

  UNIT_ASSERT_FALSE(tom->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(tom->courses.front(), art, "objects must be same");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.back(), tom, "objects must be same");

  art->students.remove(tom);

  UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.back(), jane, "objects must be same");

  jane->courses.clear();

  UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");
}

void RelationTestUnit::test_has_many_to_many_remove()
{
//  std::cout << "\n";
  matador::object_store store;

  store.attach<person>("person");
  store.attach<student, person>("student");
  store.attach<course>("course");

  auto jane = store.insert(new student("jane"));
  auto tom = store.insert(new student("tom"));
  auto art = store.insert(new course("art"));

  UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
  UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");

  jane->courses.push_back(art); // jane (value) must be push_back to course art (owner) students!!

  UNIT_ASSERT_FALSE(jane->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(jane->courses.front(), art, "objects must be same");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.front(), jane, "objects must be same");

  art->students.push_back(tom);

  UNIT_ASSERT_FALSE(tom->courses.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL, "vector size must be one");
  UNIT_ASSERT_EQUAL(tom->courses.front(), art, "objects must be same");
  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.back(), tom, "objects must be same");
  UNIT_ASSERT_EQUAL(tom.reference_count(), 1UL, "reference count must be 1");

//  std::cout << "remove tom from container\n";
  art->students.remove(tom);
  UNIT_ASSERT_EQUAL(tom.reference_count(), 0UL, "reference count must be 0");

//  std::cout << "remove tom from store\n";
  store.remove(tom);

  UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
  UNIT_ASSERT_EQUAL(art->students.back(), jane, "objects must be same");
}
