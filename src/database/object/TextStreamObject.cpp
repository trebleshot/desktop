//
// Created by veli on 2/14/19.
//

#include "TextStreamObject.h"

TextStreamObject::TextStreamObject(int id, const QString &text, QObject *parent)
        : DatabaseObject(parent), id(id), text(text)
{

}

SqlSelection *TextStreamObject::getWhere()
{
    auto *selection = new SqlSelection;

    selection->setTableName(DbStructure::TABLE_CLIPBOARD);
    selection->setWhere(QString("`%1` = ?").arg(DbStructure::FIELD_CLIPBOARD_ID));
    selection->whereArgs << this->id;

    return selection;
}

QSqlRecord TextStreamObject::getValues(AccessDatabase *db)
{
    QSqlRecord record = DbStructure::gatherTableModel(db, this)->record();

    record.setValue(DbStructure::FIELD_CLIPBOARD_ID, this->id);
    record.setValue(DbStructure::FIELD_CLIPBOARD_TEXT, this->text);
    record.setValue(DbStructure::FIELD_CLIPBOARD_TIME, (qlonglong) this->dateCreated);

    return record;
}

void TextStreamObject::onGeneratingValues(const QSqlRecord &record)
{
    this->text = record.value(DbStructure::FIELD_CLIPBOARD_TEXT).toString();
    this->dateCreated = record.value(DbStructure::FIELD_CLIPBOARD_TIME).toLongLong();
    this->id = record.value(DbStructure::FIELD_CLIPBOARD_ID).toInt();
}
