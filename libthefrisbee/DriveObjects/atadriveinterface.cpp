#include "atadriveinterface.h"

#include "frisbeeexception.h"
#include "structures/private/smartattribute.h"
#include <QCoroDBusPendingCall>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDateTime>

struct AtaDriveInterfacePrivate {
        QDBusObjectPath path;

        bool smartSupported = false;
        bool smartEnabled = false;
        bool smartFailing = false;
        QString smartSelfTestStatus;
        int smartSelfTestPercentRemaining = 0;
        quint64 smartLastUpdated;
};

AtaDriveInterface::AtaDriveInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface(path, interfaceName(), parent) {
    d = new AtaDriveInterfacePrivate();
    d->path = path;

    qDBusRegisterMetaType<SmartAttribute>();

    bindPropertyUpdater("SmartSupported", [this](QVariant value) {
        d->smartSupported = value.toBool();
    });
    bindPropertyUpdater("SmartEnabled", [this](QVariant value) {
        d->smartEnabled = value.toBool();
        emit smartEnabledChanged(value.toBool());
    });
    bindPropertyUpdater("SmartFailing", [this](QVariant value) {
        d->smartFailing = value.toBool();
        emit smartFailingChanged(value.toBool());
    });
    bindPropertyUpdater("SmartSelftestStatus", [this](QVariant value) {
        d->smartSelfTestStatus = value.toString();
        emit smartSelfTestStatusChanged(smartSelfTestStatus());
    });
    bindPropertyUpdater("SmartSelftestPercentRemaining", [this](QVariant value) {
        d->smartSelfTestPercentRemaining = value.toInt();
        emit smartSelfTestPercentRemainingChanged(value.toInt());
    });
    bindPropertyUpdater("SmartUpdated", [this](QVariant value) {
        d->smartLastUpdated = value.toULongLong();
        emit smartLastUpdatedChanged(smartLastUpdated());
    });
}

AtaDriveInterface::~AtaDriveInterface() {
    delete d;
}

QString AtaDriveInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.Drive.Ata");
}

bool AtaDriveInterface::smartSupported() {
    return d->smartSupported;
}

bool AtaDriveInterface::smartEnabled() {
    return d->smartEnabled;
}

bool AtaDriveInterface::smartFailing() {
    return d->smartFailing;
}

AtaDriveInterface::SmartSelfTestStatus AtaDriveInterface::smartSelfTestStatus() {
    static QMap<QString, SmartSelfTestStatus> statuses({
        {"success",          SmartSelfTestStatus::Success        },
        {"aborted",          SmartSelfTestStatus::Aborted        },
        {"interrupted",      SmartSelfTestStatus::Interrupted    },
        {"fatal",            SmartSelfTestStatus::Fatal          },
        {"error_unknown",    SmartSelfTestStatus::ErrorUnknown   },
        {"error_electrical", SmartSelfTestStatus::ErrorElectrical},
        {"error_servo",      SmartSelfTestStatus::ErrorServo     },
        {"error_read",       SmartSelfTestStatus::ErrorRead      },
        {"error_handling",   SmartSelfTestStatus::ErrorHandling  },
        {"inprogress",       SmartSelfTestStatus::InProgress     },
    });
    return statuses.value(d->smartSelfTestStatus, SmartSelfTestStatus::Fatal);
}

int AtaDriveInterface::smartSelfTestPercentRemaining() {
    return d->smartSelfTestPercentRemaining;
}

QDateTime AtaDriveInterface::smartLastUpdated() {
    return QDateTime::fromSecsSinceEpoch(d->smartLastUpdated);
}

QCoro::Task<> AtaDriveInterface::setSmartEnabled(bool enabled) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SmartSetEnabled");
    message.setArguments({enabled, QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

QCoro::Task<QList<FormattedSmartAttributePtr>> AtaDriveInterface::smartGetAttributes() {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SmartGetAttributes");
    message.setArguments({QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());

    auto arg = reply.arguments().first().value<QDBusArgument>();

    QList<SmartAttribute> attributes;
    arg >> attributes;

    QList<FormattedSmartAttributePtr> formattedAttributes;
    for (auto attr : attributes) {
        formattedAttributes.append(FormattedSmartAttributePtr(new FormattedSmartAttribute(attr)));
    }
    co_return formattedAttributes;
}

QCoro::Task<> AtaDriveInterface::startSmartSelfTest(SmartSelfTestType type) {
    QString typeString;
    switch (type) {
        case SmartSelfTestType::Short:
            typeString = QStringLiteral("short");
            break;
        case SmartSelfTestType::Extended:
            typeString = QStringLiteral("extended");
            break;
        case SmartSelfTestType::Conveyance:
            typeString = QStringLiteral("conveyance");
            break;
    }

    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SmartSelftestStart");
    message.setArguments({typeString, QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

QCoro::Task<> AtaDriveInterface::abortSmartSelfTest() {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SmartSelftestAbort");
    message.setArguments({QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

DiskInterface::Interfaces AtaDriveInterface::interfaceType() {
    return AtaDrive;
}
