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
#include "elementitemeditor.h"
#include "ui/qetelementeditor.h"
#include "editorcommands.h"
#include "graphicspart/customelementgraphicpart.h"
#include "graphicspart/parttext.h"
#include "graphicspart/partdynamictextfield.h"

#include "../GenericTableView/lib/generictableview.h"
#include "../GenericTableView/lib/generictablemodel.h"
#include "../GenericTableView/lib/Wrapper/propertyselectionlineedit.h"
#include "../GenericTableView/lib/Wrapper/propertyselectionspinbox.h"

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
    mUserPropertiesTableView = new GenericTableView(this);
    mUserPropertiesModel = new GenericTableModel(mUserPropertiesTableView);
    QStringList header = {"Name", "Value"};
    mUserPropertiesModel->setHeader(header);
    mUserPropertiesTableView->setModel(mUserPropertiesModel);

    vbox->addWidget(mUserPropertiesTableView);

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

    // TODO: Update list with all the user properties

    mUserPropertiesModel->clear();

    // Take the userproperties of the first
    const QHash<QString, QVariant>* hash = currentPart()->userProperties();
    QHashIterator<QString, QVariant> iterator(*hash);
    QVariant value;
    Property p;
    while(iterator.findNext(value))
    {
        iterator.next();
        p.m_name = iterator.key();
        p.m_value = iterator.value();
        p.m_datatype = PropertiesInterface::QVariantTypeToString(p.m_value);
        mUserPropertiesModel->appendProperty(p);
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
    //
}

namespace  {
    /*!
     * \brief The PropertyDialog class
     * Simple dialog to select the datatype and the name of the property
     */
    class PropertyDialog: public QDialog
    {
    public:
        PropertyDialog(QWidget *parent = nullptr): QDialog(parent) {
            QVBoxLayout* layout = new QVBoxLayout();

            cb = new QComboBox(this);
            cb->addItems(PropertiesInterface::supportedDatatypes());
            layout->addWidget(cb);

            le = new QLineEdit("Property name", this);
            layout->addWidget(le);

            QPushButton* btn = new QPushButton("OK");
            layout->addWidget(btn);

            // capture a reference (&)
            connect(btn, &QPushButton::clicked, [&] { this->accept(); });

            this->setLayout(layout);
        };

        QString name() {return le->text();}
        QString datatype() {return cb->currentText();}

    private:
        QLineEdit* le;
        QComboBox* cb;
    };
}

void ElementItemEditor::addProperty()
{
    PropertyDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString name = dialog.name();
    QString datatype = dialog.datatype();

    Property p(name, datatype);

    if (datatype == "String")
    {
        p.wrapper = QSharedPointer<PropertySelectionWrapper>(new PropertySelectionLineEdit(this));
    }
    else if (datatype == "Integer")
    {
        p.wrapper = QSharedPointer<PropertySelectionWrapper>(new PropertySelectionSpinBox(this));
    }

    mUserPropertiesModel->appendProperty(p);
}

void ElementItemEditor::removeCurrSelectedProperty()
{
    mUserPropertiesModel->removeProperty(mUserPropertiesTableView->currentIndex().row());
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
    } else if (type == CustomElementPart::Type::Text) {

        auto t = dynamic_cast<QGraphicsTextItem*>(part);
        if (t->type() == QGraphicsItem::UserType + 1110) {
            // PartDynamicTextField
            auto pd = static_cast<PartDynamicTextField*>(t);
            m_change_connections << connect(pd, &PartDynamicTextField::userPropertiesChanged, this, &ElementItemEditor::updateUserProperties);

        }
        else if (t->type() == QGraphicsItem::UserType + 1107) {
            // PartText
            auto pt = static_cast<PartText*>(t);
            m_change_connections << connect(pt, &PartText::userPropertiesChanged, this, &ElementItemEditor::updateUserProperties);
        }

    } else {
        qDebug() << "ElementItemEditor::setUpChangeConnections(): Should be unreachable!";
    }
    setUpChangeConnectionsPriv();
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
