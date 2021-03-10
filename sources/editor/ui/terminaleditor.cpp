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
#include "terminaleditor.h"
#include "ui_terminaleditor.h"
#include "../../qeticons.h"
#include "../../qet.h"
#include "../graphicspart/partterminal.h"
#include "../../QPropertyUndoCommand/qpropertyundocommand.h"

/**
 * @brief TerminalEditor::TerminalEditor
 * Default constructor
 * @param editor : element editor of which this terminal editor belong
 * @param parent : parent widget
 */
TerminalEditor::TerminalEditor(QETElementEditor *editor, QWidget *parent) :
	ElementItemEditor(editor, parent),
	ui(new Ui::TerminalEditor)
{

    ui->setupUi(editorWidget());
	init();
}

/**
 * @brief TerminalEditor::~TerminalEditor
 * Destructor
 */
TerminalEditor::~TerminalEditor()
{
	delete ui;
}

/**
 * @brief TerminalEditor::updateFormPriv
 * Reimplemented from ElementItemEditor
 * Update the content of this widget
 */
void TerminalEditor::updateFormPriv()
{
	if (!m_part) {
		return;
	}
	activeConnections(false);

	ui->m_x_dsb->setValue(m_part->property("x").toReal());
	ui->m_y_dsb->setValue(m_part->property("y").toReal());
	ui->m_orientation_cb->setCurrentIndex(ui->m_orientation_cb->findData(m_part->property("orientation")));
	ui->m_name_le->setText(m_part->name());
	ui->m_type_cb->setCurrentIndex(ui->m_orientation_cb->findData(m_part->terminalType()));

	activeConnections(true);
}

/**
 * @brief TerminalEditor::setPart
 * Set the part to edit.
 * The part must be a PartTerminal, in other case return false.
 * @param new_part : the part to edit
 * @return true if the part can be edited.
 */
bool TerminalEditor::setPart(CustomElementPart *new_part)
{
	if (m_part == new_part) {
		return true;
	}

    disconnectChangeConnections();

    m_terminals.clear();
	if (!new_part)
	{
		m_part = nullptr;
		return(true);
	}

	if (PartTerminal *part_terminal = dynamic_cast<PartTerminal *>(new_part))
	{
		m_part = part_terminal;
		updateForm();
        setUpChangeConnections();
        m_terminals.append(part_terminal);
		return(true);
	}
	return(false);
}

bool TerminalEditor::setParts(QList<CustomElementPart *> parts)
{
    m_terminals.clear();
    disconnectChangeConnections();
    if (parts.isEmpty())
    {
        m_part = nullptr;
        return(true);
    }

    if (PartTerminal *part_terminal = static_cast<PartTerminal *>(parts.first()))
    {
        m_part = part_terminal;
        m_terminals.clear();
        m_terminals.append(part_terminal);
        for (int i=1; i < parts.length(); i++)
            m_terminals.append(static_cast<PartTerminal*>(parts[i]));
        updateForm();
        setUpChangeConnections();
        return(true);
    }
    return(false);
}

/**
 * @brief TerminalEditor::currentPart
 * @return the current edited part
 * or nullptr if there is no part or several part
 * @see QList<CustomElementPart *> TerminalEditor::currentParts() const
 */
CustomElementPart *TerminalEditor::currentPart() const
{
	return m_part;
}

QList<CustomElementPart*> TerminalEditor::currentParts() const {
    QList<CustomElementPart*> parts;
    for (auto term: m_terminals) {
        parts.append(static_cast<CustomElementPart*>(term));
    }
    return parts;
}

/**
 * @brief TerminalEditor::init
 * Some init about this class
 */
void TerminalEditor::init()
{
    ui->m_orientation_cb->addItem(QET::Icons::North, tr("Nord"),  Qet::North);
    ui->m_orientation_cb->addItem(QET::Icons::East,  tr("Est"),   Qet::East);
    ui->m_orientation_cb->addItem(QET::Icons::South, tr("Sud"),   Qet::South);
	ui->m_orientation_cb->addItem(QET::Icons::West,  tr("Ouest"), Qet::West);

	ui->m_type_cb->addItem(tr("Générique"),         TerminalData::Generic);
	ui->m_type_cb->addItem(tr("Bornier intérieur"), TerminalData::Inner);
	ui->m_type_cb->addItem(tr("Bornier extérieur"), TerminalData::Outer);
}

void TerminalEditor::xPosEdited() {
    if (m_locked) return;
    m_locked = true;
    QPointF new_pos(ui->m_x_dsb->value(), 0);

    for (int i=0; i < m_terminals.length(); i++) {
        PartTerminal* term = m_terminals[i];
        new_pos.setY(term->pos().y()); // change only x value
        if (term->pos() != new_pos) {
            QPropertyUndoCommand *undo = new QPropertyUndoCommand(term, "pos", term->property("pos"), new_pos);
            undo->setText(tr("Déplacer une borne"));
            undo->enableAnimation();
            undoStack().push(undo);
        }
    }
    m_locked=false;
}

void TerminalEditor::yPosEdited() {
    if (m_locked) return;
    m_locked = true;
    QPointF new_pos(0, ui->m_y_dsb->value()); // change only y value

    for (int i=0; i < m_terminals.length(); i++) {
        PartTerminal* term = m_terminals[i];
        new_pos.setX(term->pos().x());
        if (term->pos() != new_pos) {
            QPropertyUndoCommand *undo = new QPropertyUndoCommand(term, "pos", term->property("pos"), new_pos);
            undo->setText(tr("Déplacer une borne"));
            undo->enableAnimation();
            undoStack().push(undo);
        }
    }
    m_locked=false;
}

/**
 * @brief TerminalEditor::orientationEdited
 */
void TerminalEditor::orientationEdited()
{
    if (m_locked) return;
        m_locked = true;

    QVariant var(ui->m_orientation_cb->currentData());

    for (int i=0; i < m_terminals.length(); i++) {
        PartTerminal* term = m_terminals[i];
        if (var != term->property("orientation"))
        {
            QPropertyUndoCommand *undo = new QPropertyUndoCommand(term, "orientation", term->property("orientation"), var);
            undo->setText(tr("Modifier l'orientation d'une borne"));
            undoStack().push(undo);
        }
    }
    m_locked = false;
}

/**
 * @brief TerminalEditor::nameEdited
 */
void TerminalEditor::nameEdited()
{
	if (m_locked) {
		return;
	}
    // TODO: should the name changed for all?
	m_locked = true;
	QString name_(ui->m_name_le->text());

	if (m_part->name() != name_)
	{
		auto undo = new QPropertyUndoCommand(m_part, "name", m_part->property("name"), name_);
		undo->setText(tr("Modifier le nom du terminal"));
		undoStack().push(undo);
	}
	m_locked=false;
}

/**
 * @brief TerminalEditor::typeEdited
 */
void TerminalEditor::typeEdited()
{
    if (m_locked) return;
        m_locked = true;

    QVariant type(ui->m_type_cb->currentData());

    for (int i=0; i < m_terminals.length(); i++) {
        PartTerminal* term = m_terminals[i];
        if (type != term->property("terminal_type"))
        {
            QPropertyUndoCommand *undo = new QPropertyUndoCommand(term, "terminal_type", term->property("terminal_type"), type);
            undo->setText(tr("Modifier l'orientation d'une borne"));
            undoStack().push(undo);
        }
    }
    m_locked = false;
}

/**
 * @brief TerminalEditor::activeConnections
 * Active connection between the widgets used in this editor
 * and method of this class.
 * @param active
 */
void TerminalEditor::activeConnections(bool active)
{
	if (active) {
		m_editor_connections << connect(ui->m_x_dsb, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
                                        this, &TerminalEditor::xPosEdited);
		m_editor_connections << connect(ui->m_y_dsb, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
                                        this, &TerminalEditor::yPosEdited);
		m_editor_connections << connect(ui->m_orientation_cb,  QOverload<int>::of(&QComboBox::activated),
										this, &TerminalEditor::orientationEdited);
		m_editor_connections << connect(ui->m_name_le, &QLineEdit::editingFinished,
										this, &TerminalEditor::nameEdited);
		m_editor_connections << connect(ui->m_type_cb, QOverload<int>::of(&QComboBox::activated),
										this, &TerminalEditor::typeEdited);
	} else {
		for (auto const & con : qAsConst(m_editor_connections)) {
			QObject::disconnect(con);
		}
		m_editor_connections.clear();
	}
}

void TerminalEditor::setUpChangeConnectionsPriv()
{
    m_change_connections << connect(m_part, &PartTerminal::xChanged, this, &TerminalEditor::updateForm);
    m_change_connections << connect(m_part, &PartTerminal::yChanged, this, &TerminalEditor::updateForm);
    m_change_connections << connect(m_part, &PartTerminal::orientationChanged, this, &TerminalEditor::updateForm);
    m_change_connections << connect(m_part, &PartTerminal::nameChanged, this, &TerminalEditor::updateForm);
    m_change_connections << connect(m_part, &PartTerminal::terminalTypeChanged, this, &TerminalEditor::updateForm);
}
