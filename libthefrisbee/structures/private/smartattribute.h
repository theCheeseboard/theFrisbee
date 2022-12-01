#ifndef SMARTATTRIBUTE_H
#define SMARTATTRIBUTE_H

#include <QVariantMap>

class QDBusArgument;

struct SmartAttribute {
        uchar id;
        QString name;
        quint16 flags;
        int value;
        int worst;
        int threshold;
        qint64 pretty;
        int prettyUnit;
        QVariantMap expansion;
};
Q_DECLARE_METATYPE(SmartAttribute)

QDBusArgument& operator<<(QDBusArgument& argument, const SmartAttribute& attr);
const QDBusArgument& operator>>(const QDBusArgument& argument, SmartAttribute& attr);

#endif // SMARTATTRIBUTE_H
