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

#include "matador/db/sqlite/sqlite_connection.hpp"
#include "matador/db/sqlite/sqlite_statement.hpp"
#include "matador/db/sqlite/sqlite_result.hpp"
#include "matador/db/sqlite/sqlite_types.hpp"
#include "matador/db/sqlite/sqlite_exception.hpp"

#include "matador/sql/sql.hpp"

#include <sqlite3.h>

using namespace std::placeholders;

namespace matador {
  
namespace sqlite {
  
void throw_error(int ec, sqlite3 *db, const std::string &source)
{
  if (ec == SQLITE_OK) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << sqlite3_errmsg(db);
  throw sqlite_exception(msg.str()); 
}

sqlite_connection::sqlite_connection()
  : sqlite_db_(0)
{
}

sqlite_connection::~sqlite_connection()
{
  close();
}


void sqlite_connection::open(const std::string &db)
{
  int ret = sqlite3_open(db.c_str(), &sqlite_db_);
  if (ret != SQLITE_OK) {
    throw sqlite_exception("couldn't open sql: " + db);
  }
}

bool sqlite_connection::is_open() const
{
  return sqlite_db_ != 0;
}

void sqlite_connection::close()
{
  int ret = sqlite3_close(sqlite_db_);
  
  throw_error(ret, sqlite_db_, "sqlite_close");

  sqlite_db_ = 0;
}

sqlite3* sqlite_connection::handle()
{
  return sqlite_db_;
}

void sqlite_connection::begin()
{
  std::unique_ptr<sqlite_result> res(static_cast<sqlite_result*>(execute("BEGIN TRANSACTION;")));
}

void sqlite_connection::commit()
{
  std::unique_ptr<sqlite_result> res(static_cast<sqlite_result*>(execute("COMMIT TRANSACTION;")));
}

void sqlite_connection::rollback()
{
  std::unique_ptr<sqlite_result> res(static_cast<sqlite_result*>(execute("ROLLBACK TRANSACTION;")));
}

std::string sqlite_connection::type() const
{
  return "sqlite";
}

std::string sqlite_connection::version() const
{
  return SQLITE_VERSION;
}

matador::detail::result_impl* sqlite_connection::execute(const matador::sql &sql)
{
  std::string stmt = dialect_.direct(sql);
  return execute(stmt);
}

matador::detail::result_impl* sqlite_connection::execute(const std::string &stmt)
{
  std::unique_ptr<sqlite_result> res(new sqlite_result);
  char *errmsg = 0;
  int ret = sqlite3_exec(sqlite_db_, stmt.c_str(), parse_result, res.get(), &errmsg);
  if (ret != SQLITE_OK) {
    std::string error(errmsg);
    sqlite3_free(errmsg);
    throw sqlite_exception(error);
  }
  return res.release();
}

matador::detail::statement_impl *sqlite_connection::prepare(const matador::sql &sql)
{
  std::string stmt(dialect_.prepare(sql));
  return new sqlite_statement(*this, stmt);
}

bool sqlite_connection::exists(const std::string &tablename)
{
  std::string stmt("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND tbl_name='" + tablename + "' LIMIT 1");
  std::unique_ptr<sqlite_result> res(static_cast<sqlite_result*>(execute(stmt)));

  if (res->result_rows() != 1) {
    return false;
  } else {
    char *end;
    return strtoul(res->column(0), &end, 10) == 1;
  }
}

std::vector<field> sqlite_connection::describe(const std::string &table)
{
  std::string stmt("PRAGMA table_info(" + table + ")");
  std::unique_ptr<sqlite_result> res(static_cast<sqlite_result*>(execute(stmt)));

  std::vector<field> fields;

  do {
    field f;
    char *end = nullptr;
    f.index(strtoul(res->column(0), &end, 10));
    f.name(res->column(1));
    f.type(dialect_.string_type(res->column(2)));
    end = nullptr;
    f.not_null(strtoul(res->column(3), &end, 10) == 0);
    f.default_value(res->column(4));
//    end = nullptr;
//    f.is_primary_key(strtoul(res->column(3), &end, 10) == 0);
    fields.push_back(f);
  } while (res->fetch());

  return fields;
}

basic_dialect *sqlite_connection::dialect()
{
  return &dialect_;
}

int sqlite_connection::parse_result(void* param, int column_count, char** values, char** /*columns*/)
{
  sqlite_result *result = static_cast<sqlite_result*>(param);

  /********************
   *
   * a new row was retrieved
   * add a new row to the result
   * and iterator over all columns
   * adding each column value as
   * string to the row
   *
   ********************/

  result->push_back(values, column_count);
  return 0;
}

unsigned long sqlite_connection::last_inserted_id()
{
  return static_cast<unsigned long>(sqlite3_last_insert_rowid(sqlite_db_));
}

}

}

extern "C"
{
  OOS_SQLITE_API matador::connection_impl* create_database()
  {
    return new matador::sqlite::sqlite_connection();
  }

  OOS_SQLITE_API void destroy_database(matador::connection_impl *db)
  {
    delete db;
  }
}
