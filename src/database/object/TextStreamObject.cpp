//
// Created by veli on 2/14/19.
//

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
