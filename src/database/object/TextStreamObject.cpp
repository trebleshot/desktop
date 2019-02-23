//
// Created by veli on 2/14/19.
//

#include "TextStreamObject.h"

TextStreamObject::TextStreamObject(int id, const QString &text, QObject *parent)
        : DatabaseObject(parent), id(id), text(text)
{

}

SqlSelection TextStreamObject::getWhere() const
{
    SqlSelection selection;

    selection.setTableName(DbStructure::TABLE_CLIPBOARD);
    selection.setWhere(QString("`%1` = ?").arg(DbStructure::FIELD_CLIPBOARD_ID));
    selection.whereArgs << this->id;

    return selection;
}

DbObjectMap TextStreamObject::getValues() const
{
    return DbObjectMap{
            {DbStructure::FIELD_CLIPBOARD_ID,   this->id},
            {DbStructure::FIELD_CLIPBOARD_TEXT, this->text},
            {DbStructure::FIELD_CLIPBOARD_TIME, (qlonglong) this->dateCreated}
    };
}

void TextStreamObject::onGeneratingValues(const QSqlRecord &record)
{
    this->text = record.value(DbStructure::FIELD_CLIPBOARD_TEXT).toString();
    this->dateCreated = record.value(DbStructure::FIELD_CLIPBOARD_TIME).toLongLong();
    this->id = record.value(DbStructure::FIELD_CLIPBOARD_ID).toInt();
}
