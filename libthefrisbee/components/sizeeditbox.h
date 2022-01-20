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
#ifndef SIZEEDITBOX_H
#define SIZEEDITBOX_H

#include <QValidator>
#include <QLineEdit>

struct SizeEditBoxPrivate;
class SizeEditBox : public QLineEdit {
        Q_OBJECT
    public:
        explicit SizeEditBox(QWidget* parent = nullptr);
        ~SizeEditBox();

        bool hasValidSize();
        quint64 size();

    signals:

    private:
        SizeEditBoxPrivate* d;
};

struct SizeEditValidatorPrivate;
class SizeEditValidator : public QValidator {
        Q_OBJECT
    public:
        explicit SizeEditValidator(QObject* parent = nullptr);
        ~SizeEditValidator();

    private:
        SizeEditValidatorPrivate* d;

        // QValidator interface
    public:
        State validate(QString& input, int& pos) const;
        void fixup(QString& input) const;
};

#endif // SIZEEDITBOX_H
