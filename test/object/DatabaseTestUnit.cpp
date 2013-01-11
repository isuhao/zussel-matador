#include "DatabaseTestUnit.hpp"

#include "Item.hpp"

#include "object/object_view.hpp"
#include "object/object_list.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"
#include "database/database_exception.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace oos;
using namespace std;

DatabaseTestUnit::DatabaseTestUnit()
  : unit_test("database test unit")
{
  add_test("open", std::tr1::bind(&DatabaseTestUnit::open, this), "open database test");
  add_test("simple", std::tr1::bind(&DatabaseTestUnit::simple, this), "simple database test");
  add_test("complex", std::tr1::bind(&DatabaseTestUnit::with_sub, this), "object with sub object database test");
  add_test("list", std::tr1::bind(&DatabaseTestUnit::with_list, this), "object with object list database test");
  add_test("vector", std::tr1::bind(&DatabaseTestUnit::with_vector, this), "object with object vector database test");
  add_test("reload", std::tr1::bind(&DatabaseTestUnit::reload, this), "reload database test");
  add_test("list2", std::tr1::bind(&DatabaseTestUnit::with_list2, this), "object with object list database test");
  add_test("reload_container", std::tr1::bind(&DatabaseTestUnit::reload_container, this), "reload object list database test");
}

DatabaseTestUnit::~DatabaseTestUnit()
{}

void
DatabaseTestUnit::initialize()
{
  ostore_.insert_prototype<Item>("item");
  ostore_.insert_prototype<ObjectItem<Item>, Item>("object_item");
  ostore_.insert_prototype<ItemPtrList>("item_ptr_list");
  ostore_.insert_prototype<album>("album");
  ostore_.insert_prototype<playlist>("playlist");
  ostore_.insert_prototype<track>("track");

  // delete db
  std::remove("test.sqlite");
}

void
DatabaseTestUnit::finalize()
{
  ostore_.clear(true);
}

void
DatabaseTestUnit::open()
{
  // create database and make object store known to the database
  session db(ostore_, "sqlite://test.sqlite");

  UNIT_ASSERT_TRUE(db.is_open(), "couldn't open database database");
  
  db.close();

  UNIT_ASSERT_FALSE(db.is_open(), "couldn't close database database");
}

void
DatabaseTestUnit::simple()
{
  // create database and make object store known to the database
  session db(ostore_, "sqlite://test.sqlite");

  // load data
  db.create();

  // load data
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();

    // ... do some object modifications
    typedef object_ptr<Item> item_ptr;
    typedef object_view<Item> item_view;
    // insert new object
    item_ptr item = ostore_.insert(new Item("Hello World", 70));
    UNIT_ASSERT_GREATER(item->id(), 0, "item has invalid object id");
    tr.commit();

    tr.begin();
    // modify object
    item->set_int(120);
    UNIT_ASSERT_EQUAL(item->get_int(), 120, "item has invalid int value");
    
    transaction tr2(db);
    try {
      // begin inner transaction
      tr2.begin();
      // change name again
      item->set_int(170);

      UNIT_ASSERT_EQUAL(item->get_int(), 170, "item has invalid int value");
      // rollback transaction
      tr2.rollback();

      UNIT_ASSERT_EQUAL(item->get_int(), 120, "item has invalid int value");
    } catch (exception &ex) {
      UNIT_WARN("transaction [" << tr2.id() << "] rolled back: " << ex.what());
      tr2.rollback();
    }
    tr.rollback();

    UNIT_ASSERT_EQUAL(item->get_int(), 70, "item has invalid int value");

    tr.begin();
    // delete object
    UNIT_ASSERT_TRUE(ostore_.remove(item), "couldn't delete item");

    tr.rollback();

    item_view view(ostore_);

    UNIT_ASSERT_FALSE(view.empty(), "item view is empty");
    UNIT_ASSERT_EQUAL((int)view.size(), 1, "more than one item in view");

    item = view.front();

    UNIT_ASSERT_EQUAL(item->get_string(), "Hello World", "invalid item name");
    UNIT_ASSERT_EQUAL(item->get_int(), 70, "invalid item int value");

    tr.begin();

    UNIT_ASSERT_TRUE(ostore_.remove(item), "couldn't delete item");
    
    tr.commit();

    UNIT_ASSERT_TRUE(view.empty(), "item view is empty");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("transaction [" << tr.id() << "] rolled back: " << ex.what());
    tr.rollback();
  }
  // close db
  db.close();
}

void
DatabaseTestUnit::with_sub()
{
  // create database and make object store known to the database
  session db(ostore_, "sqlite://test.sqlite");

  // load data
  db.create();

  // load data
  /****************
   *
   * comment this statement and the following
   * will happen if data exists:
   * data won't be load
   * a car object with id 1 is created
   * and an exception on insert is thrown
   * because there is already a car object
   * with id 1
   ****************/
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    typedef ObjectItem<Item> object_item_t;
    typedef object_ptr<object_item_t> object_item_ptr;
    typedef object_ptr<Item> item_ptr;
    // insert new object
    object_item_ptr object_item = ostore_.insert(new object_item_t("Foo", 42));

    UNIT_ASSERT_GREATER(object_item->id(), 0, "invalid object item");

    item_ptr item = object_item->ptr();

    UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

    item->set_int(120);
    item->set_string("Bar");

    UNIT_ASSERT_EQUAL(item->get_int(), 120, "invalid item int value");
    UNIT_ASSERT_EQUAL(item->get_string(), "Bar", "invalid item string value");

    tr.commit();
    
    UNIT_ASSERT_EQUAL(item->get_int(), 120, "invalid item int value");
    UNIT_ASSERT_EQUAL(item->get_string(), "Bar", "invalid item string value");

    tr.begin();
    
    object_view<object_item_t> oview(ostore_);

    UNIT_ASSERT_FALSE(oview.empty(), "object item view couldn't be empty");

    UNIT_ASSERT_TRUE(ostore_.remove(object_item), "couldn't remove object item");

    UNIT_ASSERT_TRUE(oview.empty(), "object item view must be empty");
    
    tr.rollback();

    UNIT_ASSERT_FALSE(oview.empty(), "object item view couldn't be empty");

  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  // close db
  db.close();
}

void
DatabaseTestUnit::with_list()
{
  typedef object_ptr<ItemPtrList> itemlist_ptr;
  typedef ItemPtrList::value_type item_ptr;

  // create database and make object store known to the database
  session db(ostore_, "sqlite://test.sqlite");

  // load data
  db.create();

  // load db
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications

    itemlist_ptr itemlist = ostore_.insert(new ItemPtrList);

    UNIT_ASSERT_GREATER(itemlist->id(), 0, "invalid item list");
    UNIT_ASSERT_TRUE(itemlist->empty(), "item list must be empty");

    tr.commit();

    tr.begin();
    for (int i = 0; i < 2; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new Item(name.str()));

      UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

      itemlist->push_back(item);
    }

    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");

    tr.rollback();

    UNIT_ASSERT_TRUE(itemlist->empty(), "item list must be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 0, "invalid item list size");

    tr.begin();

    for (int i = 0; i < 2; ++i) {
      stringstream name;
      name << "Item " << i+1;
      item_ptr item = ostore_.insert(new Item(name.str()));

      UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

      itemlist->push_back(item);
    }

    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");

    tr.commit();
    
    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");
    tr.begin();
    
    itemlist->clear();

    UNIT_ASSERT_TRUE(itemlist->empty(), "item list must be empty");

    tr.rollback();

    UNIT_ASSERT_FALSE(itemlist->empty(), "item list couldn't be empty");
    UNIT_ASSERT_EQUAL((int)itemlist->size(), 2, "invalid item list size");

  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  // close db
  db.close();
}

void
DatabaseTestUnit::with_list2()
{
  ostore_.insert_prototype<person>("PERSON");
  ostore_.insert_prototype<employee, person>("EMPLOYEE");
  ostore_.insert_prototype<department>("DEPARTMENT");
  
  typedef object_ref<department> dep_ref;
  typedef object_ref<employee> emp_ref;
  
  dep_ref identpro = ostore_.insert(new department("IdentPro"));
  
  emp_ref emp = ostore_.insert(new employee("Sascha"));
  identpro->add(emp);
  emp = ostore_.insert(new employee("David"));
  identpro->add(emp);
  emp = ostore_.insert(new employee("Dieter"));
  identpro->add(emp);
  emp = ostore_.insert(new employee("Stefan"));
  identpro->add(emp);
  
  department::const_iterator first = identpro->begin();
  department::const_iterator last = identpro->end();
  while (first != last) {
    first++;
//    cout << "\n" << (*first++)->value()->name();
  }
}

void
DatabaseTestUnit::with_vector()
{
}

void
DatabaseTestUnit::reload()
{
  typedef ObjectItem<Item> object_item_t;
  typedef object_ptr<object_item_t> object_item_ptr;
  typedef object_ptr<Item> item_ptr;
  typedef object_view<object_item_t> oview_t;

  // create database and make object store known to the database
  session db(ostore_, "sqlite://test.sqlite");

  try {
    // load data
    db.create();

    // load data
    db.load();
  } catch (exception &) {
    UNIT_FAIL("couldn't create and load database");
  }

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();
    // ... do some object modifications
    // insert new object
    object_item_ptr object_item = ostore_.insert(new object_item_t("Foo", 42));

    UNIT_ASSERT_GREATER(object_item->id(), 0, "invalid object item");

    item_ptr item = object_item->ptr();

    UNIT_ASSERT_GREATER(item->id(), 0, "invalid item");

    item->set_int(120);
    item->set_string("Bar");

    UNIT_ASSERT_EQUAL(item->get_int(), 120, "invalid item int value");
    UNIT_ASSERT_EQUAL(item->get_string(), "Bar", "invalid item string value");

    item->set_cstr("hello", 5);

    oview_t oview(ostore_);

    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");

    tr.commit();
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  // close db
  db.close();

  // clear object store
  ostore_.clear();

  db.open();
  
  // load data
  db.load();

  try {
    oview_t oview(ostore_);

    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "object view must not be empty");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  // close db
  db.close();
}

void
DatabaseTestUnit::reload_container()
{
  typedef object_ptr<album> album_ptr;
  typedef object_ptr<track> track_ptr;
  
  // create database and make object store known to the database
  session db(ostore_, "sqlite://test.sqlite");

  // load data
  db.create();

  // load db
  db.load();

  // create new transaction    
  transaction tr(db);
  try {
    // begin transaction
    tr.begin();

    album_ptr alb1 = ostore_.insert(new album("My Album"));

    UNIT_ASSERT_GREATER(alb1->id(), 0, "invalid album");
    UNIT_ASSERT_TRUE(alb1->empty(), "album must be empty");

    tr.commit();

    tr.begin();
    for (int i = 0; i < 100000; ++i) {
      stringstream name;
      name << "Track " << i+1;

      track_ptr trk = ostore_.insert(new track(name.str()));
      UNIT_ASSERT_GREATER(trk->id(), 0, "invalid track");

      alb1->add(trk);
//      cout << "added track: " << trk->title() << " (index: " << trk->index() << ")\n";
    }

    alb1->insert(alb1->begin() + 2, ostore_.insert(new track("Track 6")));

    tr.commit();

    /*
    cout << "\n";

    album::const_iterator first = alb1->begin();
    album::const_iterator last = alb1->end();
    while (first != last) {
      track_ptr tr1 = *first++;
      cout << "track: " << tr1->title() << " (index: " << tr1->index() << ")\n";
    }
    */

    cout << "\nsize: " << alb1->size() << "\n";

    UNIT_ASSERT_FALSE(alb1->empty(), "album couldn't be empty");
    UNIT_ASSERT_EQUAL((int)alb1->size(), 100001, "invalid album size");
  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }

  std::ofstream out("prototype.dot", ios_base::out | ios_base::trunc);
  
  ostore_.dump_prototypes(out);

  // close db
  db.close();  
  // clear object store
  ostore_.clear();

  db.open();
  
  // load data
  db.load();

  try {
    typedef object_view<album> album_view_t;
    album_view_t oview(ostore_);

    UNIT_ASSERT_TRUE(oview.begin() != oview.end(), "album view must not be empty");
    
    album_ptr alb1 = *oview.begin();
    
    cout << "size: " << alb1->size() << "\n";
    
    UNIT_ASSERT_FALSE(alb1->empty(), "album couldn't be empty");
    UNIT_ASSERT_EQUAL((int)alb1->size(), 100001, "invalid album size");

    /*
    album::const_iterator first = alb1->begin();
    album::const_iterator last = alb1->end();
    while (first != last) {
      track_ptr tr1 = *first++;
      cout << "track: " << tr1->title() << " (index: " << tr1->index() << ")\n";
    }
    */

  } catch (database_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught database exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  } catch (object_exception &ex) {
    // error, abort transaction
    UNIT_WARN("caught object exception: " << ex.what() << " (start rollback)");
    tr.rollback();
  }
  // close db
  db.close();
}
