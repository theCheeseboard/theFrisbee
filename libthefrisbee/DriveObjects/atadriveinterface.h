#ifndef ATADRIVEINTERFACE_H
#define ATADRIVEINTERFACE_H

#include "diskinterface.h"
#include "structures/formattedsmartattribute.h"
#include <QCoroTask>
#include <QDateTime>

class SmartAttribute;
class QDBusObjectPath;
struct AtaDriveInterfacePrivate;
class AtaDriveInterface : public DiskInterface {
        Q_OBJECT
    public:
        explicit AtaDriveInterface(QDBusObjectPath path, QObject* parent = nullptr);
        ~AtaDriveInterface();

        enum class SmartSelfTestStatus {
            Success,
            Aborted,
            Interrupted,
            Fatal,
            ErrorUnknown,
            ErrorElectrical,
            ErrorServo,
            ErrorRead,
            ErrorHandling,
            InProgress
        };

        enum class SmartSelfTestType {
            Short,
            Extended,
            Conveyance
        };

        static QString interfaceName();

        bool smartSupported();
        bool smartEnabled();
        bool smartFailing();
        SmartSelfTestStatus smartSelfTestStatus();
        int smartSelfTestPercentRemaining();
        QDateTime smartLastUpdated();

        QCoro::Task<> setSmartEnabled(bool enabled);
        QCoro::Task<QList<FormattedSmartAttributePtr>> smartGetAttributes();
        QCoro::Task<> startSmartSelfTest(SmartSelfTestType type);
        QCoro::Task<> abortSmartSelfTest();

    signals:
        void smartEnabledChanged(bool enabled);
        void smartFailingChanged(bool failing);
        void smartSelfTestStatusChanged(SmartSelfTestStatus status);
        void smartSelfTestPercentRemainingChanged(int remaining);
        void smartLastUpdatedChanged(QDateTime lastUpdated);

    private:
        AtaDriveInterfacePrivate* d;

        // DiskInterface interface
    public:
        Interfaces interfaceType();
};

#endif // ATADRIVEINTERFACE_H
