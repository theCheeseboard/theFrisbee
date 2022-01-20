/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "sizeeditbox.h"

struct SizeEditBoxPrivate {
    static QMap<QString, quint64> factors;
};

#define BYTE_FACTOR 1ULL
#define KILOBYTE_FACTOR 1024ULL
#define MEGABYTE_FACTOR KILOBYTE_FACTOR * KILOBYTE_FACTOR
#define GIGABYTE_FACTOR MEGABYTE_FACTOR * KILOBYTE_FACTOR
#define TERABYTE_FACTOR GIGABYTE_FACTOR * KILOBYTE_FACTOR
#define PETABYTE_FACTOR TERABYTE_FACTOR * KILOBYTE_FACTOR
#define EXABYTE_FACTOR PETABYTE_FACTOR * KILOBYTE_FACTOR

QMap<QString, quint64> SizeEditBoxPrivate::factors = {
    {"B", BYTE_FACTOR},
    {"K", KILOBYTE_FACTOR},
    {"KB", KILOBYTE_FACTOR},
    {"KIB", KILOBYTE_FACTOR},
    {"M", MEGABYTE_FACTOR},
    {"MB", MEGABYTE_FACTOR},
    {"MIB", MEGABYTE_FACTOR},
    {"G", GIGABYTE_FACTOR},
    {"GB", GIGABYTE_FACTOR},
    {"GIB", GIGABYTE_FACTOR},
    {"T", TERABYTE_FACTOR},
    {"TB", TERABYTE_FACTOR},
    {"TIB", TERABYTE_FACTOR},
    {"P", PETABYTE_FACTOR},
    {"PB", PETABYTE_FACTOR},
    {"PIB", PETABYTE_FACTOR},
    {"E", EXABYTE_FACTOR},
    {"EB", EXABYTE_FACTOR},
    {"EIB", EXABYTE_FACTOR}
};

SizeEditBox::SizeEditBox(QWidget* parent) : QLineEdit(parent) {
    d = new SizeEditBoxPrivate();
    this->setValidator(new SizeEditValidator(this));
}

SizeEditBox::~SizeEditBox() {
    delete d;
}

bool SizeEditBox::hasValidSize() {
    QString validation = this->text();
    int pos = 0;
    bool acceptable = this->validator()->validate(validation, pos) == QValidator::Acceptable;
    if (validation != this->text()) this->setText(validation);
    return acceptable;
}

quint64 SizeEditBox::size() {
    QString input = this->text();

    QString number;
    QString unit;
    for (int i = 0; i < input.length(); i++) {
        if (!input.at(i).isDigit() && input.at(i) != QLocale().decimalPoint()) {
            number = input.left(i).trimmed();
            unit = input.mid(i).toUpper().trimmed();
            break;
        }
    }

    double size = number.toDouble() * SizeEditBoxPrivate::factors.value(unit);
    return size;
}

struct SizeEditValidatorPrivate {

};

SizeEditValidator::SizeEditValidator(QObject* parent) : QValidator(parent) {
    d = new SizeEditValidatorPrivate();
}

SizeEditValidator::~SizeEditValidator() {
    delete d;
}

QValidator::State SizeEditValidator::validate(QString& input, int& pos) const {
    if (input.isEmpty()) return Intermediate;
    if (input.endsWith(QLocale().decimalPoint())) return Intermediate;

    bool ok;
    input.toDouble(&ok);
    if (ok) return Intermediate;

    QString number;
    QString unit;
    for (int i = 0; i < input.length(); i++) {
        if (!input.at(i).isDigit() && input.at(i) != QLocale().decimalPoint()) {
            number = input.left(i).trimmed();
            unit = input.mid(i).toUpper();
            if (i + 1 == pos && !unit.startsWith(" ")) pos++;
            unit = unit.trimmed();
            break;
        }
    }

    if (number.isEmpty()) number = input;
    number.toDouble(&ok);
    if (!ok) return Invalid;

    if (unit.isEmpty()) return Acceptable;

    if (!SizeEditBoxPrivate::factors.contains(unit)) return Invalid;

    input = QStringLiteral("%1 %2").arg(number, unit);

    if (unit.length() != 2 && unit != "B") return Intermediate;
    return Acceptable;
}


void SizeEditValidator::fixup(QString& input) const {
    QString number;
    QString unit;
    for (int i = 0; i < input.length(); i++) {
        if (!input.at(i).isDigit() && input.at(i) != QLocale().decimalPoint()) {
            number = input.left(i).trimmed();
            unit = input.mid(i).toUpper().trimmed();
            break;
        }
    }

    if (number.isEmpty()) number = input;

    if (unit.isEmpty()) unit = "B";
    if (QStringList({"K", "KIB"}).contains(unit.toUpper())) unit = "KB";
    if (QStringList({"M", "MIB"}).contains(unit.toUpper())) unit = "MB";
    if (QStringList({"G", "GIB"}).contains(unit.toUpper())) unit = "GB";
    if (QStringList({"T", "TIB"}).contains(unit.toUpper())) unit = "TB";
    if (QStringList({"P", "PIB"}).contains(unit.toUpper())) unit = "PB";
    if (QStringList({"E", "EIB"}).contains(unit.toUpper())) unit = "EB";

    input = QStringLiteral("%1 %2").arg(number, unit);
}
