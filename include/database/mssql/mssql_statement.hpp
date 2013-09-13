/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MSSQL_STATEMENT_HPP
#define MSSQL_STATEMENT_HPP

#include "database/statement.hpp"
#include "database/sql.hpp"

#include "database/mssql/mssql_exception.hpp"

#if defined(_MSC_VER)
#include <windows.h>
#endif

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <string>
#include <vector>
#include <type_traits>
#include <iostream>

#ifdef WIN32
#define CPP11_TYPE_TRAITS_NS std::tr1
#else
#define CPP11_TYPE_TRAITS_NS std
#endif

namespace oos {

class database;

namespace mssql {

class mssql_database;

class mssql_statement : public statement
{
public:
  explicit mssql_statement(mssql_database &db);
  mssql_statement(mssql_database &db, const sql &s);
  virtual ~mssql_statement();

  virtual void clear();
  virtual result* execute();
  virtual void prepare(const sql &s);
  virtual void reset();
  
//  virtual int column_count() const;
//  virtual const char* column_name(int i) const;

  virtual database& db();
  virtual const database& db() const;

  static long type2int(data_type_t type);
  static long type2sql(data_type_t type);

protected:
  virtual void write(const char *id, char x);
  virtual void write(const char *id, short x);
  virtual void write(const char *id, int x);
  virtual void write(const char *id, long x);
  virtual void write(const char *id, unsigned char x);
  virtual void write(const char *id, unsigned short x);
  virtual void write(const char *id, unsigned int x);
  virtual void write(const char *id, unsigned long x);
  virtual void write(const char *id, float x);
  virtual void write(const char *id, double x);
  virtual void write(const char *id, bool x);
	virtual void write(const char *id, const char *x, int s);
  virtual void write(const char *id, const varchar_base &x);
  virtual void write(const char *id, const std::string &x);
	virtual void write(const char *id, const object_base_ptr &x);
  virtual void write(const char *id, const object_container &x);

  template < class T >
  void bind_value(T val, int index)
  {
    SQLLEN len = 0;
    int ctype = mssql_statement::type2int(type_traits<T>::data_type());
    int type = mssql_statement::type2sql(type_traits<T>::data_type());
    SQLRETURN ret = SQLBindParameter(stmt_, index, SQL_PARAM_INPUT, ctype, type, 0, 0, &val, 0, &len);
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
  }
  void bind_value(const char *val, int size, int index);

  virtual void prepare_result_column(const sql::field_ptr &fptr);

private:
  mssql_database &db_;
  int result_size;
  int host_size;
  std::vector<bool> host_data;
  
  SQLHANDLE stmt_;
};

}

}

#endif /* MSSQL_STATEMENT_HPP */
