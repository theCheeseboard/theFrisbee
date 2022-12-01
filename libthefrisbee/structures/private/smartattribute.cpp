#include "smartattribute.h"

#include <QDBusArgument>

QDBusArgument& operator<<(QDBusArgument& argument, const SmartAttribute& attr) {
    argument.beginStructure();
    argument << attr.id << attr.name << attr.flags << attr.value << attr.worst << attr.threshold << attr.pretty << attr.prettyUnit << attr.expansion;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, SmartAttribute& attr) {
    argument.beginStructure();
    argument >> attr.id >> attr.name >> attr.flags >> attr.value >> attr.worst >> attr.threshold >> attr.pretty >> attr.prettyUnit >> attr.expansion;
    argument.endStructure();
    return argument;
}
