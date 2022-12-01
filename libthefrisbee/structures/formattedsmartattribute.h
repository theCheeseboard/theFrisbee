#ifndef FORMATTEDSMARTATTRIBUTE_H
#define FORMATTEDSMARTATTRIBUTE_H

#include <QObject>

class SmartAttribute;
struct FormattedSmartAttributePrivate;
class FormattedSmartAttribute : public QObject {
        Q_OBJECT
    public:
        explicit FormattedSmartAttribute(SmartAttribute attribute, QObject* parent = nullptr);
        ~FormattedSmartAttribute();

        uchar id();
        QString name();
        QString displayName();
        QString displayValue();
        int value();
        int worst();
        int threshold();

        bool isPreFailAttribute();
        bool isOnlineAttribute();

    signals:

    private:
        FormattedSmartAttributePrivate* d;
};

typedef QSharedPointer<FormattedSmartAttribute> FormattedSmartAttributePtr;

#endif // FORMATTEDSMARTATTRIBUTE_H
