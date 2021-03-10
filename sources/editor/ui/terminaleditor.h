/*
        Copyright 2006-2021 The QElectroTech Team
        This file is part of QElectroTech.

        QElectroTech is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 2 of the License, or
        (at your option) any later version.

        QElectroTech is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TERMINALEDITOR_H
#define TERMINALEDITOR_H

#include <QWidget>
#include "../elementitemeditor.h"

namespace Ui {
	class TerminalEditor;
}

class PartTerminal;

/**
 * @brief The TerminalEditor class
 * Provide a widget used to edit the properties of a PartTerminal
 */
class TerminalEditor : public ElementItemEditor
{
    Q_OBJECT

    public:
		TerminalEditor(QETElementEditor *editor, QWidget *parent = nullptr);
		~TerminalEditor() override;

        void updateFormPriv() override;
		bool setPart(CustomElementPart *new_part) override;
        bool setParts(QList<CustomElementPart *> parts);
		CustomElementPart *currentPart() const override;
        QList<CustomElementPart*> currentParts() const override;

    private:
		void init();
        void xPosEdited();
        void yPosEdited();
		void orientationEdited();
		void nameEdited();
		void typeEdited();
		void activeConnections(bool active);
        void setUpChangeConnectionsPriv() override;

    private:
        Ui::TerminalEditor *ui;
        QVector<QMetaObject::Connection> m_editor_connections;
        PartTerminal *m_part{nullptr};
        QList <PartTerminal *> m_terminals;
		bool m_locked = false;
};

#endif // TERMINALEDITOR_H
