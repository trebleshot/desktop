//
// Created by veli on 2/24/19.
//

#pragma once

#include <QObject>

class Interrupter {

    bool m_interrupted = false;

public:
    bool interrupted();

    void interrupt();

    void reset();
};