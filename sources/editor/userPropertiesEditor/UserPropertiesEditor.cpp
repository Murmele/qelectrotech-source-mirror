#include "UserPropertiesEditor.h"
#include "GenericTableView/lib/generictableview.h"
#include "GenericTableView/lib/generictablemodel.h"
#include "GenericTableView/lib/generictabledelegator.h"
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
#include <QSortFilterProxyModel>
#include <QUndoStack>

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

class AddRemoveUserPropertiesModelUndoCommand: public QUndoCommand {
public:
	enum Action {
		Remove,
		Add
	};
	AddRemoveUserPropertiesModelUndoCommand(GenericTableModel* model, const Property& p, Action a):
	mModel(model), mProperty(p), mAction(a){}

	void redo() {
		if (mAction== Action::Add) {
			mModel->appendProperty(&mProperty);
		} else if (mAction == Action::Remove) {
			mModel->removeProperty(mProperty);
		}
	}

	void undo() {
		if (mAction== Action::Add) {
			mModel->removeProperty(mProperty);
		} else {
			mModel->appendProperty(&mProperty);
		}
	}
private:
	GenericTableModel* mModel;
	Property mProperty;
	Action mAction;
};

class UpdateUserPropertiesModelUndoCommand: public QUndoCommand {
public:
	enum Action {
		Remove,
		Add
	};
	UpdateUserPropertiesModelUndoCommand(GenericTableModel* model, const Property& p):
	mModel(model), mNewProperty(p){}

	void redo() {
		auto p = mModel->property(mNewProperty.m_name);
		if (!p)
			validProperty = false;
		else {
			validProperty = true;
			mOldProperty = *p;
		}
		mModel->updateProperty(mNewProperty);
	}

	void undo() {
		if (validProperty)
			mModel->updateProperty(mOldProperty);
	}
private:
	GenericTableModel* mModel;
	Property mOldProperty;
	Property mNewProperty;
	bool validProperty{false};
};
}

/*!
 * \brief The ProjectElementUserPropertiesProxy class
 * Proxy to show only the name and the value of the property. Otherwise also the datatype and the required flag are shown
 */
class LimitNameValueProxy: public QSortFilterProxyModel
{
public:
    LimitNameValueProxy(QObject *parent = nullptr): QSortFilterProxyModel(parent) {}
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const  override {
        Q_UNUSED(source_parent);
        if (source_column <= 1)
            return true;
        return false;
    }

};

UserPropertiesEditor::UserPropertiesEditor(QWidget *parent, bool LimitNameValue, QUndoStack* stack): QWidget(parent), mUndoStack(stack)
{
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);


    mUserPropertiesTableView = new GenericTableView(this);
    if (LimitNameValue) {
        auto proxy = new LimitNameValueProxy(mUserPropertiesTableView);
        mUserPropertiesModel = new GenericTableModel(proxy);
        proxy->setSourceModel(mUserPropertiesModel);
        mUserPropertiesTableView->setModel(proxy);
    } else {
        mUserPropertiesModel = new GenericTableModel(mUserPropertiesTableView);
        mUserPropertiesTableView->setModel(mUserPropertiesModel);
    }
    mUserPropertiesDelegator = new GenericTableDelegator(mUserPropertiesTableView);
    mUserPropertiesTableView->setItemDelegate(mUserPropertiesDelegator);

    QStringList header = {tr("Name"), tr("Value"), tr("Datatype"), tr("Required")};
    mUserPropertiesModel->setHeader(header);

	QHBoxLayout* h = new QHBoxLayout();
	mAdd = new QPushButton(tr("Add"), this);
	h->addWidget(mAdd);
	mRemove = new QPushButton(tr("Remove"), this);
	h->addWidget(mRemove);
	h->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QVBoxLayout* l = new QVBoxLayout();
	l->addLayout(h);
    l->addWidget(mUserPropertiesTableView);

    setLayout(l);

    connect(mUserPropertiesModel, &GenericTableModel::propertyAdded, this, &UserPropertiesEditor::propertyAdded);
    connect(mUserPropertiesModel, &GenericTableModel::propertyRemoved, this, &UserPropertiesEditor::propertyRemoved);
    connect(mUserPropertiesModel, &GenericTableModel::propertyUpdated, this, &UserPropertiesEditor::propertyUpdated);
	connect(mAdd, &QPushButton::clicked, this, QOverload<>::of(&UserPropertiesEditor::addProperty));
	connect(mRemove, &QPushButton::clicked, this, &UserPropertiesEditor::removeSelectedProperty);


    for (auto prop: PropertiesInterface::supportedDatatypes())
		registerDatatype(prop);
}

void UserPropertiesEditor::registerDatatype(const QString& datatype)
{
    if (mUserPropertiesDelegator->wrapperWidget(datatype))
        return; // Datatype is already linked to a Wrapper, so no need to do it again

    if (datatype == PropertiesInterface::stringS)
        mUserPropertiesDelegator->registerWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionLineEdit(this)));
    else if (datatype == PropertiesInterface::integerS)
        mUserPropertiesDelegator->registerWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionSpinBox(this)));
    else if (datatype == PropertiesInterface::doubleS)
        mUserPropertiesDelegator->registerWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionDoubleSpinBox(this)));
//    else if (datatype == PropertiesInterface::colorS)
//              WrapperManager::instance()->registerWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionColor(this)));
//    else if (datatype == PropertiesInterface::boolS)
//              WrapperManager::instance()->registerWrapperWidget(datatype, QSharedPointer<PropertySelectionWrapper>(new PropertySelectionCheckBox(this)));
}

void UserPropertiesEditor::clearModel()
{
    mUserPropertiesModel->clear();
}

void UserPropertiesEditor::addProperty()
{
    PropertyDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString name = dialog.name();
    QString datatype = dialog.datatype();

	auto p = Property(name, datatype);
	auto undoCommand = new AddRemoveUserPropertiesModelUndoCommand(mUserPropertiesModel, p, AddRemoveUserPropertiesModelUndoCommand::Add);

	if (mUndoStack)
		mUndoStack->push(undoCommand);
	else {
		QUndoStack stack; // Dummy stack to execute the undo command, but no undo is possible
		stack.push(undoCommand);
	}
}

void UserPropertiesEditor::addProperty(const QString& name, const QVariant& value)
{
    Property* p = new Property();
    p->m_name = name;
    p->m_value = value;
    p->m_datatype = PropertiesInterface::QVariantTypeToString(p->m_value);
    p->m_required = false;
    mUserPropertiesModel->appendProperty(p);
}

void UserPropertiesEditor::addProperty(QHashIterator<QString, QVariant>& iterator)
{
    while(iterator.hasNext())
    {
        iterator.next();
        addProperty(iterator.key(), iterator.value());
    }
}


void UserPropertiesEditor::setProperties(QHashIterator<QString, QVariant>& iterator)
{
    clearModel();
    addProperty(iterator);
}

void UserPropertiesEditor::removeSelectedProperty()
{

	auto* p = mUserPropertiesModel->property(mUserPropertiesTableView->currentIndex().row());
	if (!p)
		return;
	auto undoCommand = new AddRemoveUserPropertiesModelUndoCommand(mUserPropertiesModel, *p, AddRemoveUserPropertiesModelUndoCommand::Remove);

	if (mUndoStack)
		mUndoStack->push(undoCommand);
	else {
		QUndoStack stack; // Dummy stack to execute the undo command, but no undo is possible
		stack.push(undoCommand);
	}
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

void UserPropertiesEditor::setProxy(QSortFilterProxyModel *model)
{
	model->setSourceModel(mUserPropertiesModel);
	mUserPropertiesTableView->setModel(model);
}
