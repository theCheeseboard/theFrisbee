/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "opticalerrortracker.h"

#include <QMap>

struct OpticalErrorTrackerPrivate {
    bool detectedError = false;

    int senseKey;
    int asc;
    int ascq;

    QString currentError;

    const QMap<quint16, QMap<quint16, QMap<quint16, const char*>>> senseDescriptions = {
        {
            0x3, {
                //Medium Error
                {
                    0x30, {
                        {0x09, QT_TR_NOOP("the last time the disc was burned, it was not burned with support for adding extra files")}
                    }
                },
                {
                    0x3A, {
                        {0x00, QT_TR_NOOP("there is no disc in the drive")}
                    }
                },
                {
                    0x55, {
                        {0x0B, QT_TR_NOOP("the drive requires more power to operate")}
                    }
                },
                {
                    0x72, {
                        {0x00, QT_TR_NOOP("the disc couldn't be finalized")}
                    }
                },
                {
                    0x73, {
                        {0x02, QT_TR_NOOP("the power calibration area is full")},
                        {0x03, QT_TR_NOOP("the drive can't calibrate the laser power level for this disc")}
                    }
                }
            }
        }
    };
};

OpticalErrorTracker::OpticalErrorTracker(QObject* parent) : QObject(parent) {
    d = new OpticalErrorTrackerPrivate();
}

OpticalErrorTracker::~OpticalErrorTracker() {
    delete d;
}

void OpticalErrorTracker::sendLine(QString line) {
    if (line.startsWith("Sense Bytes:")) {
        QStringList senseBytesString = line.split(" ", Qt::SkipEmptyParts);
        senseBytesString.takeFirst();
        senseBytesString.takeFirst();

        QList<int> senseBytes;
        senseBytes.reserve(senseBytesString.length());
        for (QString senseByte : senseBytesString) {
            senseBytes.append(senseByte.toInt(nullptr, 16));
        }

        d->senseKey = senseBytes.at(2) & 0x0F;
        d->asc = senseBytes.at(12);
        d->ascq = senseBytes.at(13);

        const char* senseDescription = d->senseDescriptions.value(d->senseKey).value(d->asc).value(d->ascq, nullptr);
        if (senseDescription != nullptr) {
            d->currentError = tr(senseDescription);
            d->detectedError = true;
        }
    }
}

bool OpticalErrorTracker::detectedError() {
    return d->detectedError;
}

QString OpticalErrorTracker::errorReason() {
    return d->currentError;
}
