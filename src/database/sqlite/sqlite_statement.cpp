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

#include "database/sqlite/sqlite_statement.hpp"
#include "database/sqlite/sqlite_database.hpp"

#include "database/row.hpp"

#include <sqlite3.h>

namespace oos {

namespace sqlite {

sqlite_statement::sqlite_statement(sqlite_database &db)
  : stmt_(0)
  , db_(db)
{
}

sqlite_statement::~sqlite_statement()
{
  finalize();
}

result* sqlite_statement::execute(const std::string &sql)
{
  return 0;
}

row* sqlite_statement::step()
{
  int ret = sqlite3_step(stmt_);
  if (ret == SQLITE_ROW) {
    // retrieved new row
    // create row object
    row *r = new row;
    
    return r;
  } else if (ret == SQLITE_DONE) {
    // no further row available
    return 0;
  } else {
    // error
  }
  return 0;
}

int sqlite_statement::prepare(const std::string &sql)
{
  finalize();
  int ret = sqlite3_prepare_v2(db_(), sql.c_str(), sql.size(), &stmt_, 0);
  if (SQLITE_OK != ret) {
//    throw error;
  }
  return ret;
}

int sqlite_statement::finalize()
{
  return sqlite3_finalize(stmt_);
}

int sqlite_statement::reset()
{
  return sqlite3_reset(stmt_);
}

int sqlite_statement::column_count() const
{
  return sqlite3_column_count(stmt_);
}

const char* sqlite_statement::column_text(int i) const
{
  return (const char*)sqlite3_column_text(stmt_, i);
}

int sqlite_statement::column_int(int i) const
{
  return sqlite3_column_int(stmt_, i);
}

double sqlite_statement::column_double(int i) const
{
  return sqlite3_column_double(stmt_, i);
}

int sqlite_statement::column_type(int i) const
{
  return sqlite3_column_type(stmt_, i);
}

int sqlite_statement::bind(int i, double value)
{
  return sqlite3_bind_double(stmt_, i, value);
}

int sqlite_statement::bind(int i, int value)
{
  return sqlite3_bind_int(stmt_, i, value);
}

int sqlite_statement::bind(int i, const std::string &value)
{
  return sqlite3_bind_text(stmt_, i, value.c_str(), value.size(), 0);
}

}

}