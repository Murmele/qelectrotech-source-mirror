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
#include "../utils/qetutils.h"
#include "elementitemeditor.h"
#include "ui/qetelementeditor.h"
#include "editorcommands.h"
#include "graphicspart/customelementgraphicpart.h"
#include "graphicspart/parttext.h"
#include "graphicspart/partdynamictextfield.h"
#include "userPropertiesEditor/UserPropertiesEditor.h"
#include "userPropertiesEditor/GenericTableView/lib/property.h"
#include "userpropertyundocommand.h"

/**
	Constructeur
	@param editor QETElementEditor auquel cet editeur appartient
	@param parent QWidget parent de cet editeur
*/
ElementItemEditor::ElementItemEditor(QETElementEditor *editor, QWidget *parent) :
	QWidget(parent),
	element_editor(editor)
{
    QVBoxLayout* vbox = new QVBoxLayout();

    // Dummy widget on which all individual editors will add their needed
    // widgets
    mEditorWidget = new QWidget(this);
    vbox->addWidget(mEditorWidget);

    QLabel* l = new QLabel(tr("User Properties:"), this);
    vbox->addWidget(l);


    // This widget is always the last
    mUserPropertiesEditor = new UserPropertiesEditor(this);
    vbox->addWidget(mUserPropertiesEditor);

    QHBoxLayout* addRem = new QHBoxLayout();
    QPushButton* add = new QPushButton(tr("Add"), this);
    connect(add, &QPushButton::pressed, this, &ElementItemEditor::addProperty);
    QPushButton* rem = new QPushButton(tr("Remove"), this);
    connect(rem, &QPushButton::pressed, this, &ElementItemEditor::removeCurrSelectedProperty);
    addRem->addWidget(add);
    addRem->addWidget(rem);


    vbox->addLayout(addRem);

    setLayout(vbox);
}

/// @return le QETElementEditor auquel cet editeur appartient
QETElementEditor *ElementItemEditor::elementEditor() const
{
	return(element_editor);
}

/// @return l'ElementScene contenant les parties editees par cet editeur
ElementScene *ElementItemEditor::elementScene() const
{
	return(element_editor -> elementScene());
}

void ElementItemEditor::disconnectChangeConnections()
{
    for (QMetaObject::Connection c : m_change_connections) {
    disconnect(c);
    }
    m_change_connections.clear();
}

void ElementItemEditor::updateForm()
{


    // Take the userproperties of the first
    CustomElementPart* part = currentPart();
    if (part)
    {
        QHashIterator<QString, QVariant> iterator = part->userProperties();
        disconnectWidgetToEditorConnections();
        mUserPropertiesEditor->setProperties(iterator);
        setupWidgetToEditorConnections();
    }

    updateFormPriv();
}

/// @return la QUndoStack a utiliser pour les annulations
QUndoStack &ElementItemEditor::undoStack() const
{
	return(elementScene() -> undoStack());
}

/// @return Le nom du type d'element edite
QString ElementItemEditor::elementTypeName() const
{
	return(element_type_name);
}

/// @param name Nom du type d'element edite
void ElementItemEditor::setElementTypeName(const QString &name) {
	element_type_name = name;
}

void ElementItemEditor::updateUserProperties(const QString& key)
{
    if (mIgnoreElementPropertyChanges)
        return;

    assert(currentPart());
    if (!currentPart()->existUserProperty(key))
        return;

    mUserPropertiesEditor->updateProperty(key, currentPart()->userProperty(key));
}

void ElementItemEditor::addProperty()
{
    mUserPropertiesEditor->addProperty();
}

void ElementItemEditor::removeCurrSelectedProperty()
{
    mUserPropertiesEditor->removeSelectedProperty();
}

void ElementItemEditor::userPropertyAdded(const QString& name)
{
    auto parts = currentParts();
    if (parts.isEmpty())
        return;

    QETUtils::Lock lock(mIgnoreElementPropertyChanges);
    QVariant value = mUserPropertiesEditor->property(name)->m_value;
    undoStack().push(new AddUserPropertyUndoCommand(parts, name, value));
}

void ElementItemEditor::userPropertyRemoved(const QString& name)
{
    auto parts = currentParts();
    if (parts.isEmpty())
        return;

    QETUtils::Lock lock(mIgnoreElementPropertyChanges);
    undoStack().push(new RemoveUserPropertyUndoCommand(parts, name));
}

void ElementItemEditor::userPropertyUpdated(const QString& name)
{
    auto parts = currentParts();
    if (parts.isEmpty())
        return;

    QETUtils::Lock lock(mIgnoreElementPropertyChanges);
    QVariant value = mUserPropertiesEditor->property(name)->m_value;
    undoStack().push(new UpdateUserPropertyUndoCommand(parts, name, value));
}

void ElementItemEditor::setUpChangeConnections()
{

    CustomElementPart* part = currentPart();
    CustomElementPart::Type type = part->elementType();

    // Problem is that PartText and PartDynamicTextEdit derive not from
    // Custom ElementGraphicPart
    if (type == CustomElementPart::Type::ElementGraphics) {
        auto g = dynamic_cast<CustomElementGraphicPart*>(part);
        m_change_connections << connect(g, &CustomElementGraphicPart::userPropertiesChanged, this, &ElementItemEditor::updateUserProperties);
        m_change_connections << connect(g, &CustomElementGraphicPart::userPropertyAdded, this, &ElementItemEditor::updateUserProperties);
        m_change_connections << connect(g, &CustomElementGraphicPart::userPropertyRemoved, this, &ElementItemEditor::updateUserProperties);
    } else if (type == CustomElementPart::Type::Text) {

        auto t = dynamic_cast<QGraphicsTextItem*>(part);
        if (t->type() == QGraphicsItem::UserType + 1110) {
            // PartDynamicTextField
            auto pd = static_cast<PartDynamicTextField*>(t);
            m_change_connections << connect(pd, &PartDynamicTextField::userPropertiesChanged, this, &ElementItemEditor::updateUserProperties);
            m_change_connections << connect(pd, &PartDynamicTextField::userPropertyAdded, this, &ElementItemEditor::updateUserProperties);
            m_change_connections << connect(pd, &PartDynamicTextField::userPropertyRemoved, this, &ElementItemEditor::updateUserProperties);

        }
        else if (t->type() == QGraphicsItem::UserType + 1107) {
            // PartText
            auto pt = static_cast<PartText*>(t);
            m_change_connections << connect(pt, &PartText::userPropertiesChanged, this, &ElementItemEditor::updateUserProperties);
            m_change_connections << connect(pt, &PartText::userPropertyAdded, this, &ElementItemEditor::updateUserProperties);
            m_change_connections << connect(pt, &PartText::userPropertyRemoved, this, &ElementItemEditor::updateUserProperties);
        }

    } else {
        qDebug() << "ElementItemEditor::setUpChangeConnections(): Should be unreachable!";
    }
    setUpChangeConnectionsPriv();
}

void ElementItemEditor::setupWidgetToEditorConnections()
{
    mWidgetsToEditorConnections << connect(mUserPropertiesEditor,
                                           &UserPropertiesEditor::propertyAdded,
                                           this, &ElementItemEditor::userPropertyAdded);
    mWidgetsToEditorConnections << connect(mUserPropertiesEditor,
                                           &UserPropertiesEditor::propertyRemoved,
                                           this, &ElementItemEditor::userPropertyRemoved);
    mWidgetsToEditorConnections << connect(mUserPropertiesEditor,
                                           &UserPropertiesEditor::propertyUpdated,
                                           this, &ElementItemEditor::userPropertyUpdated);
}

void ElementItemEditor::disconnectWidgetToEditorConnections()
{
    for (QMetaObject::Connection c : mWidgetsToEditorConnections) {
        disconnect(c);
    }
    mWidgetsToEditorConnections.clear();
}

/**
	Detache l'editeur de la primitive qu'il edite.
	Equivaut a setPart(0)
	@see setPart
*/
void ElementItemEditor::detach()
{
	setPart(nullptr);
}
