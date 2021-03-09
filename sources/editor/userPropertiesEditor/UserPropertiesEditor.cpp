#include "UserPropertiesEditor.h"
#include "GenericTableView/lib/generictableview.h"
#include "GenericTableView/lib/generictablemodel.h"
#include "GenericTableView/lib/Wrapper/propertyselectionlineedit.h"
#include "GenericTableView/lib/Wrapper/propertyselectionspinbox.h"
#include "GenericTableView/lib/Wrapper/propertyselectiondoublespinbox.h"

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
            QStringList trd = PropertiesInterface::supportedDatatypes();
            cb->addItems(translateSupportedDatatypes(trd));
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
        QString datatype() {return cb->currentText();}
    private:
        QStringList translateSupportedDatatypes(QStringList& list)
        {
            QStringList translated;
            for (auto s: list)
            {
                QString trd;
                if (s == PropertiesInterface::stringS)
                    trd = tr("Integer");
                else if (s == PropertiesInterface::doubleS)
                    trd = tr("Double");
                else if (s == PropertiesInterface::boolS)
                    trd = tr("Boolean");
                else if (s == PropertiesInterface::stringS)
                    trd = tr("String");
                else if (s == PropertiesInterface::colorS)
                    trd = tr("Color");
                else
                    trd = "";
                translated.append(trd);
            }
            return translated;
        }

    private:
        QLineEdit* le;
        QComboBox* cb;
    };
}

UserPropertiesEditor::UserPropertiesEditor(QWidget *parent): QWidget(parent)
{
    mUserPropertiesTableView = new GenericTableView(this);
    mUserPropertiesModel = new GenericTableModel(mUserPropertiesTableView);
    QStringList header = {"Name", "Value"};
    mUserPropertiesModel->setHeader(header);
    mUserPropertiesTableView->setModel(mUserPropertiesModel);
}

QSharedPointer<PropertySelectionWrapper> UserPropertiesEditor::wrapperForDatatype(QString datatype)
{
    if (datatype == PropertiesInterface::stringS)
    {
        return QSharedPointer<PropertySelectionWrapper>(new PropertySelectionLineEdit(this));
    }
    else if (datatype == PropertiesInterface::integerS)
    {
        return QSharedPointer<PropertySelectionWrapper>(new PropertySelectionSpinBox(this));
    }
    else if (datatype == PropertiesInterface::doubleS)
    {
        return QSharedPointer<PropertySelectionWrapper>(new PropertySelectionDoubleSpinBox(this));
    }
//    else if (datatype == PropertiesInterface::colorS)
//    {
//        return QSharedDataPointer(PropertySelectionWrapper>(new PropertySelectionColor(this)));
//    }

    return nullptr;
}

void UserPropertiesEditor::addProperty()
{
    PropertyDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString name = dialog.name();
    QString datatype = dialog.datatype();

    Property p(name, datatype);
    p.wrapper = wrapperForDatatype(datatype);

    mUserPropertiesModel->appendProperty(p);
}

void UserPropertiesEditor::setProperties(QHashIterator<QString, QVariant>& iterator)
{
    mUserPropertiesModel->clear();

    QVariant value;
    Property p;
    while(iterator.findNext(value))
    {
        iterator.next();
        p.m_name = iterator.key();
        p.m_value = iterator.value();
        p.m_datatype = PropertiesInterface::QVariantTypeToString(p.m_value);
        p.m_required = false;
        p.wrapper = wrapperForDatatype(p.m_datatype);
        mUserPropertiesModel->appendProperty(p);
    }
}

void UserPropertiesEditor::removeSelectedProperty()
{
    mUserPropertiesModel->removeProperty(mUserPropertiesTableView->currentIndex().row());
}
