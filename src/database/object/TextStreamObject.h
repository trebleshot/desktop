//
// Created by veli on 2/14/19.
//

#ifndef TREBLESHOT_TEXTSTREAMOBJECT_H
#define TREBLESHOT_TEXTSTREAMOBJECT_H

#include <src/database/AccessDatabase.h>

class TextStreamObject : public DatabaseObject {
Q_OBJECT
public:
    int id;
    QString text;
    time_t dateCreated;

    explicit TextStreamObject(int id = 0, const QString &text = nullptr, QObject *parent = nullptr);

    SqlSelection *getWhere() override;

    QSqlRecord getValues(AccessDatabase *db) override;

    void onGeneratingValues(const QSqlRecord &record) override;

    void onUpdatingObject(AccessDatabase *db) override
    {
        DatabaseObject::onUpdatingObject(db);
    }

    void onInsertingObject(AccessDatabase *db) override
    {
        DatabaseObject::onInsertingObject(db);
    }

    void onRemovingObject(AccessDatabase *db) override
    {
        DatabaseObject::onRemovingObject(db);
    }

};


#endif //TREBLESHOT_TEXTSTREAMOBJECT_H
