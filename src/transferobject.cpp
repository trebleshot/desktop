#include "transferobject.h"

#include <QSqlField>
#include <QSqlQuery>

TransferObject::TransferObject()
{
}

QSqlQuery TransferObject::getWhere()
{
    return QSqlQuery();
}

QList<QSqlField>* TransferObject::getValues()
{
    return new QList<QSqlField>();
}

void TransferObject::onGeneratingValues(QList<QSqlField>* db)
{
}
