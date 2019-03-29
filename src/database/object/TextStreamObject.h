//
// Created by veli on 2/14/19.
//

#pragma once

#include <src/database/AccessDatabase.h>

class TextStreamObject : public DatabaseObject {
public:
    int id;
    QString text;
    time_t dateCreated;

    explicit TextStreamObject(int id = 0, const QString &text = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};