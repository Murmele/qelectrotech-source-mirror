#ifndef USERPROPERTYUNDOCOMMAND_H
#define USERPROPERTYUNDOCOMMAND_H

#include <QUndoCommand>
#include <QVariant>

class CustomElementPart;


class UserPropertyUndoCommand: public QUndoCommand
{
public:
    UserPropertyUndoCommand(const QList<CustomElementPart*>& parts): mParts(parts) {}
      virtual void redo() = 0;
      virtual void undo() = 0;

    void addUserProperty(const QString& name, const QVariant& value);
    void removeUserProperty(const QString& name);
    void setUserProperty(const QString& name, const QVariant& value);
    void saveProperties(QList<QVariant>& values);
    void restoreProperties(const QList<QVariant>& values);
protected:
    QList<CustomElementPart*> mParts;
    QString mName;
};

class AddUserPropertyUndoCommand: public UserPropertyUndoCommand
{
public:
    AddUserPropertyUndoCommand(const QList<CustomElementPart*>& parts, const QString& name, const QVariant value):
        UserPropertyUndoCommand(parts), mValue(value) {
        mName = name;
    }
    virtual void redo() override {addUserProperty(mName, mValue);};
    virtual void undo() override {removeUserProperty(mName);};
private:
    QVariant mValue;
};

class RemoveUserPropertyUndoCommand: public UserPropertyUndoCommand
{
public:
    RemoveUserPropertyUndoCommand(const QList<CustomElementPart*>& parts, const QString& name):
        UserPropertyUndoCommand(parts) {
        mName = name;
    }
    virtual void redo()  override {
        saveProperties(mValues);
        removeUserProperty(mName);
    };
    virtual void undo() override {restoreProperties(mValues);};

private:
    QList<QVariant> mValues;
};

class UpdateUserPropertyUndoCommand: public UserPropertyUndoCommand
{
public:
    UpdateUserPropertyUndoCommand(const QList<CustomElementPart*>& parts, const QString& name, const QVariant value):
            UserPropertyUndoCommand(parts) {
            mName = name;
            mNewValue = value;
        }

    virtual void redo() override {saveProperties(mOldValues);};
    virtual void undo() override {restoreProperties(mOldValues);};

private:
    QVariant mNewValue;
    QList<QVariant> mOldValues;
};



#endif // USERPROPERTYUNDOCOMMAND_H
