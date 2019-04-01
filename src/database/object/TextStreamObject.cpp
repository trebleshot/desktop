/*
* Copyright (C) 2019 Veli Tasalı, created on 2/14/19
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "TextStreamObject.h"

TextStreamObject::TextStreamObject(int id, const QString &text)
		: DatabaseObject(), id(id), text(text)
{

}

SqlSelection TextStreamObject::getWhere() const
{
	SqlSelection selection;

	selection.setTableName(DB_TABLE_CLIPBOARD);
	selection.setWhere(QString("`%1` = ?").arg(DB_FIELD_CLIPBOARD_ID));
	selection.whereArgs << this->id;

	return selection;
}

DbObjectMap TextStreamObject::getValues() const
{
	return DbObjectMap{
			{DB_FIELD_CLIPBOARD_ID,   this->id},
			{DB_FIELD_CLIPBOARD_TEXT, this->text},
			{DB_FIELD_CLIPBOARD_TIME, (qlonglong) this->dateCreated}
	};
}

void TextStreamObject::onGeneratingValues(const DbObjectMap &record)
{
	this->text = record.value(DB_FIELD_CLIPBOARD_TEXT).toString();
	this->dateCreated = record.value(DB_FIELD_CLIPBOARD_TIME).toLongLong();
	this->id = record.value(DB_FIELD_CLIPBOARD_ID).toInt();
}
