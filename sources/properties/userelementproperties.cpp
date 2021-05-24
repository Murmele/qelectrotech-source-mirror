#include "userelementproperties.h"
#include "../qetinformation.h"

namespace  {
	const QString settingsGroup = "diagrameditor/userElementProperties";
	const QString terminalSettingsGroup = "diagrameditor/userTerminalProperties";
}

UserElementProperty::UserElementProperty(): PropertiesInterface("UserElementProperty")
{

}

UserElementProperty::UserElementProperty(const QVariant& value) : PropertiesInterface("UserElementProperty"), mValue(value)
{

}

QHash<QString, UserElementProperty> UserElementProperty::defaultProperties()
{
	QHash<QString, UserElementProperty> hash;
	QSettings settings;

	settings.beginGroup(settingsGroup);
	auto keys = settings.allKeys();
	if (keys.length() == 0) {
		// no user settings are available.
		// so set the default ones
		keys = QETInformation::elementInfoKeys();
		for (auto key: keys) {
			UserElementProperty properties;
			QVariant value = "";
			properties.setValue(value);
			hash.insert(key, properties);
		}
	} else {
		for (auto key: keys) {
			UserElementProperty property;
			property.fromSettings(settings, key);
			hash.insert(key, property);
		}
	}
	settings.endGroup();

	return hash;
}

void UserElementProperty::toSettings(QSettings &settings, const QString &key) const
{
	settings.setValue(key, mValue);
}

void UserElementProperty::fromSettings(QSettings &settings, const QString &key)
{
	mValue = settings.value(key, "");
}

void UserElementProperty::toXmlPriv (QDomElement &e) const
{
	assert(false);
	return;
}

bool UserElementProperty::fromXmlPriv (const QDomElement &e)
{
	assert(false);
	return false;
}

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
TerminalProperty::TerminalProperty(): PropertiesInterface("UserTerminalProperty")
{

}

TerminalProperty::TerminalProperty(const QVariant& value): PropertiesInterface("UserTerminalProperty"), mValue(value)
{

}

QHash<QString, TerminalProperty> TerminalProperty::defaultProperties()
{
	QHash<QString, TerminalProperty> hash;
	QSettings settings;

	settings.beginGroup(terminalSettingsGroup);
	auto keys = settings.allKeys();
	if (keys.length() == 0) {
		// no user settings are available.
		// so set the default ones
		return hash; // At the moment no default properties for the terminals
	} else {
		for (auto key: keys) {
			TerminalProperty property;
			property.fromSettings(settings, key);
			hash.insert(key, property);
		}
	}
	settings.endGroup();

	return hash;
}

void TerminalProperty::toSettings(QSettings &settings, const QString &key) const
{
	settings.setValue(key, mValue);
}

void TerminalProperty::fromSettings(QSettings &settings, const QString &key)
{
	mValue = settings.value(key, "");
}

void TerminalProperty::toXmlPriv (QDomElement &e) const
{
	assert(false);
	return;
}

bool TerminalProperty::fromXmlPriv (const QDomElement &e)
{
	assert(false);
	return false;
}
