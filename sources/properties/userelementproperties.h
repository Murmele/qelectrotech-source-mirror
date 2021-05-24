#ifndef USERELEMENTPROPERTIES_H
#define USERELEMENTPROPERTIES_H

#include "propertiesinterface.h"

/*!
 * \brief The UserElementProperty class
 * User property of an element
 */
class UserElementProperty: public PropertiesInterface
{
public:
	UserElementProperty();
    UserElementProperty(const QVariant& value);
	static QHash<QString, UserElementProperty> defaultProperties();
	virtual void toSettings(QSettings &,
							const QString & = QString()) const override;
	virtual void fromSettings(QSettings &,
							  const QString & key = QString()) override;
	virtual void toXmlPriv (QDomElement &e) const override;
	virtual bool fromXmlPriv (const QDomElement &e) override;
    QVariant value() const {return mValue;}
    void setValue(const QVariant& value) {mValue = value;}

private:
	QVariant mValue;
};

class TerminalProperty: public PropertiesInterface
{
public:
	TerminalProperty();
    TerminalProperty(const QVariant& value);
	static QHash<QString, TerminalProperty> defaultProperties();
	virtual void toSettings(QSettings &,
							const QString & = QString()) const override;
	virtual void fromSettings(QSettings &,
							  const QString & key = QString()) override;
	virtual void toXmlPriv (QDomElement &e) const override;
	virtual bool fromXmlPriv (const QDomElement &e) override;
    QVariant value() const {return mValue;}
	void setValue(QVariant& value) {mValue = value;}

private:
	QVariant mValue;
};

#endif // USERELEMENTPROPERTIES_H
