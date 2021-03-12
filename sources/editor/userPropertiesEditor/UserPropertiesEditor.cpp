#include "UserPropertiesEditor.h"
#include "GenericTableView/lib/generictableview.h"
#include "GenericTableView/lib/generictablemodel.h"
#include "GenericTableView/lib/Wrapper/propertyselectionlineedit.h"
#include "GenericTableView/lib/Wrapper/propertyselectionspinbox.h"
#include "GenericTableView/lib/Wrapper/propertyselectiondoublespinbox.h"
#include "GenericTableView/lib/wrappermanager.h"

#include "../../properties/propertiesinterface.h"

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

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
            cb->addItems(PropertiesInterface::translateSupportedDatatypes());
            layout->addWidget(cb);

            le = new QLineEdit(tr("Property name"), this);
            layout->addWidget(le);

            QPushButton* btn = new QPushButton(tr("OK"));
            layout->addWidget(btn);

            // capture a reference (&)
            connect(btn, &QPushButton::clicked, [&] { this->accept(); });

            this->setLayout(layout);
        };

        QString name() {return le->text();}
        QString datatype() {return PropertiesInterface::translationToDatatype(cb->currentText());}
    private:
        QLineEdit* le;
        QComboBox* cb;
    };
}

UserPropertiesEditor::UserPropertiesEditor(QWidget *parent): QWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);


    mUserPropertiesTableView = new GenericTableView(this);
    mUserPropertiesModel = new GenericTableModel(mUserPropertiesTableView);
    QStringList header = {"Name", "Value"};
    mUserPropertiesModel->setHeader(header);
    mUserPropertiesTableView->setModel(mUserPropertiesModel);

    QVBoxLayout* l = new QVBoxLayout();
    l->addWidget(mUserPropertiesTableView);

    setLayout(l);

    connect(mUserPropertiesModel, &GenericTableModel::propertyAdded, this, &UserPropertiesEditor::propertyAdded);
    connect(mUserPropertiesModel, &GenericTableModel::propertyRemoved, this, &UserPropertiesEditor::propertyRemoved);
    connect(mUserPropertiesModel, &GenericTableModel::propertyUpdated, this, &UserPropertiesEditor::propertyUpdated);

    for (auto prop: PropertiesInterface::supportedDatatypes())
        addDatatype(prop);
}

void UserPropertiesEditor::addDatatype(const QString& datatype)
{
    if (WrapperManager::instance()->wrapperWidget(datatype))
        return; // Datatype is already linked to a Wrapper, so no need to do it again

    if (datatype == PropertiesInterface::stringS)
        WrapperManager::instance()->addWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionLineEdit(this)));
    else if (datatype == PropertiesInterface::integerS)
        WrapperManager::instance()->addWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionSpinBox(this)));
    else if (datatype == PropertiesInterface::doubleS)
        WrapperManager::instance()->addWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionDoubleSpinBox(this)));
//    else if (datatype == PropertiesInterface::colorS)
//              WrapperManager::instance()->addWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionColor(this)));
//    else if (datatype == PropertiesInterface::boolS)
//              WrapperManager::instance()->addWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionCheckBox(this)));
}

void UserPropertiesEditor::addProperty()
{
    PropertyDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString name = dialog.name();
    QString datatype = dialog.datatype();
    mUserPropertiesModel->appendProperty(new Property(name, datatype));
}

void UserPropertiesEditor::setProperties(QHashIterator<QString, QVariant>& iterator)
{
    mUserPropertiesModel->clear();

    Property* p;
    while(iterator.hasNext())
    {
        iterator.next();
        p = new Property();
        p->m_name = iterator.key();
        p->m_value = iterator.value();
        p->m_datatype = PropertiesInterface::QVariantTypeToString(p->m_value);
        p->m_required = false;
        mUserPropertiesModel->appendProperty(p);
    }
}

void UserPropertiesEditor::removeSelectedProperty()
{
    mUserPropertiesModel->removeProperty(mUserPropertiesTableView->currentIndex().row());
}

void UserPropertiesEditor::updateProperty(const QString& key, const QVariant& value)
{
   mUserPropertiesModel->updateProperty(key, value);
}

const Property* UserPropertiesEditor::property(const QString& name) {
    if (mUserPropertiesModel)
    {
        return mUserPropertiesModel->property(name);
    }
    return nullptr;
}

const QVector<Property*> UserPropertiesEditor::properties()
{
    return mUserPropertiesModel->properties();
}
