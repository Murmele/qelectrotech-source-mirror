/*
	Copyright 2006-2021 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef QETUTILS_H
#define QETUTILS_H

#include <QMargins>

/**
	Provide some small utils function
*/
namespace QETUtils
{
	QString marginsToString(const QMargins &margins);
	QMargins marginsFromString(const QString &string);

    /*!
     * \brief The Lock class
     * Mechanism to not have to do locking and unlocking like
     * lock = true;
     * ... do something
     * lock = false;
     * but only creating a local variable of this type:
     * Lock(lock);
     * This will unlock as soon as the variable Lock will be deleted
     */
    class Lock {
    public:
        Lock(bool& variable): mVariable(variable) {mVariable = true;};
        ~Lock() {mVariable = false;}

    private:
        bool& mVariable;
    };
}

#endif // QETUTILS_H
