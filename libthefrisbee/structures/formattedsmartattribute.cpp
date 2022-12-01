#include "formattedsmartattribute.h"

#include "structures/private/smartattribute.h"
#include <QLocale>

struct FormattedSmartAttributePrivate {
        SmartAttribute attr;
};

FormattedSmartAttribute::FormattedSmartAttribute(SmartAttribute attribute, QObject* parent) :
    QObject{parent} {
    d = new FormattedSmartAttributePrivate();
    d->attr = attribute;
}

FormattedSmartAttribute::~FormattedSmartAttribute() {
    delete d;
}

uchar FormattedSmartAttribute::id() {
    return d->attr.id;
}

QString FormattedSmartAttribute::name() {
    return d->attr.name;
}

QString FormattedSmartAttribute::displayName() {
    static QMap<QString, const char*> displayNames({
        {"raw-read-error-rate",         QT_TR_NOOP("Read Error Rate")              },
        {"spin-up-time",                QT_TR_NOOP("Spin Up Time")                 },
        {"start-stop-count",            QT_TR_NOOP("Start/Stop Count")             },
        {"reallocated-sector-count",    QT_TR_NOOP("Reallocated Sector Count")     },
        {"seek-error-rate",             QT_TR_NOOP("Seek Error Rate")              },
        {"power-on-hours",              QT_TR_NOOP("Power On Hours")               },
        {"spin-retry-count",            QT_TR_NOOP("Spin Retry Count")             },
        {"power-cycle-count",           QT_TR_NOOP("Power Cycle Count")            },
        {"reported-uncorrect",          QT_TR_NOOP("Reported Uncorrectable Errors")},
        {"high-fly-writes",             QT_TR_NOOP("High Fly Writes")              },
        {"airflow-temperature-celsius", QT_TR_NOOP("Airflow Temperature")          },
        {"g-sense-error-rate",          QT_TR_NOOP("G-Sense Error Rate")           },
        {"power-off-retract-count",     QT_TR_NOOP("Power Off Retract Count")      },
        {"load-cycle-count",            QT_TR_NOOP("Load Cycle Count")             },
        {"temperature-celsius-2",       QT_TR_NOOP("Temperature")                  },
        {"hardware-ecc-recovered",      QT_TR_NOOP("Hardware ECC Recovered")       },
        {"current-pending-sector",      QT_TR_NOOP("Current Pending Sector Count") },
        {"offline-uncorrectable",       QT_TR_NOOP("Uncorrectable Sector Count")   },
        {"udma-crc-error-count",        QT_TR_NOOP("UDMA CRC Error Rate")          },
        {"head-flying-hours",           QT_TR_NOOP("Head Flying Hours")            },
        {"total-lbas-written",          QT_TR_NOOP("Total LBAs Written")           },
        {"total-lbas-read",             QT_TR_NOOP("Total LBAs Read")              },
        {"throughput-performance",      QT_TR_NOOP("Throughput Performace")        },
        {"reallocated-event-count",     QT_TR_NOOP("Reallocation Count")           },
    });

    if (displayNames.contains(d->attr.name)) {
        return tr(displayNames.value(d->attr.name));
    } else {
        return d->attr.name;
    }
}

QString FormattedSmartAttribute::displayValue() {
    switch (d->attr.prettyUnit) {
        case 1:
            return QLocale().toString(d->attr.pretty);
        case 2:
            return tr("%n minutes", nullptr, d->attr.pretty / 1000 / 60);
        case 3:
            return tr("%n sectors", nullptr, d->attr.pretty);
        case 4:
            {
                auto celsius = (d->attr.pretty / 1000.0 - 273.15);
                if (QLocale().measurementSystem() == QLocale::MetricSystem || QLocale().measurementSystem() == QLocale::ImperialUKSystem) {
                    // Convert to Celsius
                    return tr("%n °C", nullptr, celsius);
                } else {
                    // Convert to Fahrenheit
                    return tr("%n °F", nullptr, celsius * (9.0 / 5.0) + 32);
                }
            }
        case 0: // Unknown
        default:
            return tr("N/A");
    }
}

int FormattedSmartAttribute::value() {
    return d->attr.value;
}

int FormattedSmartAttribute::worst() {
    return d->attr.worst;
}

int FormattedSmartAttribute::threshold() {
    return d->attr.threshold;
}

bool FormattedSmartAttribute::isPreFailAttribute() {
    return d->attr.flags & 0x1;
}

bool FormattedSmartAttribute::isOnlineAttribute() {
    return d->attr.flags & 0x2;
}
