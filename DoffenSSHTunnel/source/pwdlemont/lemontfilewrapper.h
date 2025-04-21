#ifndef LEMONTFILEWRAPPER_H
#define LEMONTFILEWRAPPER_H

#include <QList>
#include "../identifier.h"

class LemontFileWrapper
{
public:
    static QList<Identifier> getIdentifiers(QString filename, QString password);
};

#endif // LEMONTFILEWRAPPER_H
