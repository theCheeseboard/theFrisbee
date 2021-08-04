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

QMap<QString, quint64> SizeEditBoxPrivate::factors = {
    {"B", 1ULL},
    {"K", 1024ULL},
    {"KB", 1024ULL},
    {"KIB", 1024ULL},
    {"M", 1024 * 1024ULL},
    {"MB", 1024 * 1024ULL},
    {"MIB", 1024 * 1024ULL},
    {"G", 1024 * 1024 * 1024ULL},
    {"GB", 1024 * 1024 * 1024ULL},
    {"GIB", 1024 * 1024 * 1024ULL},
    {"T", 1024 * 1024 * 1024 * 1024ULL},
    {"TB", 1024 * 1024 * 1024 * 1024ULL},
    {"TIB", 1024 * 1024 * 1024 * 1024ULL},
    {"P", 1024 * 1024 * 1024 * 1024 * 1024ULL},
    {"PB", 1024 * 1024 * 1024 * 1024 * 1024ULL},
    {"PIB", 1024 * 1024 * 1024 * 1024 * 1024ULL},
    {"E", 1024 * 1024 * 1024 * 1024 * 1024 * 1024ULL},
    {"EB", 1024 * 1024 * 1024 * 1024 * 1024 * 1024ULL},
    {"EIB", 1024 * 1024 * 1024 * 1024 * 1024 * 1024ULL}
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
