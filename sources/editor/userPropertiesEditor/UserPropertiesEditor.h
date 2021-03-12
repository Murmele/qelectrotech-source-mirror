#ifndef USERPROPERTIESEDITOR_H
#define USERPROPERTIESEDITOR_H

#include <QWidget>

class GenericTableView;
class GenericTableModel;
class PropertySelectionWrapper;
class Property;

/*!
 * \brief The UserPropertiesEditor class
 * Widget which handles the userproperties displaying and editing
 */
class UserPropertiesEditor: public QWidget
{
    Q_OBJECT
public:
    UserPropertiesEditor(QWidget* parent = nullptr);
    void setProperties(QHashIterator<QString, QVariant>& iterator);
    void addProperty();
    void removeSelectedProperty();
    void updateProperty(const QString& key, const QVariant& value);
    const Property* property(const QString& name);
    const QVector<Property*> properties();

signals:
    void propertyRemoved(const QString& name);
    void propertyUpdated(const QString& name);
    void propertyAdded(const QString& name);
private:
    void addDatatype(const QString &datatype);
private:
    GenericTableView* mUserPropertiesTableView{nullptr};
    GenericTableModel* mUserPropertiesModel{nullptr};
};

#endif /* USERPROPERTIESEDITOR_H */
