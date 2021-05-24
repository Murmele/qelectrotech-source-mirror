#ifndef USERPROPERTIESEDITOR_H
#define USERPROPERTIESEDITOR_H

#include <QWidget>

class GenericTableView;
class GenericTableModel;
class GenericTableDelegator;
class PropertySelectionWrapper;
class Property;
class QSortFilterProxyModel;
class QPushButton;
class QUndoStack;

/*!
 * \brief The UserPropertiesEditor class
 * Widget which handles the userproperties displaying and editing
 * This widget is shown when the properties of an element gets
 * modified
 */
class UserPropertiesEditor: public QWidget
{
    Q_OBJECT
public:
    UserPropertiesEditor(QWidget* parent = nullptr, bool LimitNameValue = false, QUndoStack* undostack = nullptr);
    void setProperties(QHashIterator<QString, QVariant>& iterator);
    void clearModel();
    void addProperty();
    void addProperty(const QString& name, const QVariant& value);
    void addProperty(QHashIterator<QString, QVariant>& iterator);
    void removeSelectedProperty();
    void updateProperty(const QString& key, const QVariant& value);
    const Property* property(const QString& name);
    const QVector<Property*> properties();
	/*!
	 * \brief setProxy
	 * Put proxy between model and view
	 * \param model
	 */
	void setProxy(QSortFilterProxyModel *model);

signals:
    void propertyRemoved(const QString& name);
    void propertyUpdated(const QString& name);
    void propertyAdded(const QString& name);
private:
	void registerDatatype(const QString &datatype);
private:
    GenericTableView* mUserPropertiesTableView{nullptr};
    GenericTableModel* mUserPropertiesModel{nullptr};
    GenericTableDelegator* mUserPropertiesDelegator{nullptr};
	QPushButton* mRemove{nullptr};
	QPushButton* mAdd{nullptr};
	QUndoStack* mUndoStack{nullptr};
};

#endif /* USERPROPERTIESEDITOR_H */
