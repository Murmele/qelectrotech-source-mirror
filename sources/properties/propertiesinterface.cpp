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
#include "propertiesinterface.h"
#include <QDebug>
#include "../qetxml.h"

const QString PropertiesInterface::integerS = "int";
const QString PropertiesInterface::doubleS = "double";
const QString PropertiesInterface::boolS = "bool";
const QString PropertiesInterface::stringS = "string";
const QString PropertiesInterface::uuidS = "uuid";
const QString PropertiesInterface::colorS = "color";

/*!
 * Available property types
 */
namespace  {

    const QString userPropertiesS = "userProperties";
}

UserPropertiesUndoCommand::UserPropertiesUndoCommand(const QVector<PropertiesInterface*>& objs, QHash<QString, QVariant>& newProperties, QUndoCommand *parent):
    QUndoCommand(parent),
    mObjs(objs),
    mNewProperties(newProperties)
{
    // Find differences between old and new properties
    QHashIterator it(newProperties);

    for (int i=0; i < objs.length(); i++) {
        auto curHash = objs[i]->userPropertiesHash();

        QHash<QString, QVariant> added, removed, changed;

        // find all changes
        // find all added objects
        while (it.hasNext()) {
            it.next();
            if (!curHash.contains(it.key()))
                added.insert(it.key(), it.value());
            else if (it.value() != curHash.value(it.key()) && it.value().isValid())
                changed.insert(it.key(), curHash.value(it.key()));
            else {}
        }

        // find all removed objects
        QHashIterator itOld(curHash);
        while (itOld.hasNext()) {
            itOld.next();
            if (!newProperties.contains(itOld.key()))
                removed.insert(itOld.key(), itOld.value());
        }

        mAddedProperties.append(added);
        mRemovedProperties.append(removed);
        mChangedProperties.append(changed);
    }
}

void UserPropertiesUndoCommand::redo() {
    for (int i=0; i < mObjs.length(); i++) {
        auto removeIterator = QHashIterator(mRemovedProperties[i]);
        while(removeIterator.hasNext()) {
            removeIterator.next();
            mObjs[i]->removeProperty(removeIterator.key());
        }

        auto addIterator = QHashIterator(mAddedProperties[i]);
        while(addIterator.hasNext()) {
            addIterator.next();
            mObjs[i]->setUserProperty(addIterator.key(), addIterator.value());
        }

        auto changeIterator = QHashIterator(mNewProperties);
        while(changeIterator.hasNext()) {
            changeIterator.next();
            mObjs[i]->setUserProperty(changeIterator.key(), changeIterator.value());
        }
    }
}

void UserPropertiesUndoCommand::undo() {
    for (int i=0; i < mObjs.length(); i++) {
        auto removeIterator = QHashIterator(mRemovedProperties[i]);
        while(removeIterator.hasNext()) {
            removeIterator.next();
            mObjs[i]->setUserProperty(removeIterator.key(), removeIterator.value());
        }

        auto addIterator = QHashIterator(mAddedProperties[i]);
        while(addIterator.hasNext()) {
            addIterator.next();
            mObjs[i]->removeProperty(addIterator.key());
        }

        auto changeIterator = QHashIterator(mChangedProperties[i]);
        while(changeIterator.hasNext()) {
            changeIterator.next();
            mObjs[i]->setUserProperty(changeIterator.key(), changeIterator.value());
        }
    }
}

/**
	@brief PropertiesInterface::PropertiesInterface
*/
PropertiesInterface::PropertiesInterface(const QString &tagname):
    mTagName(tagname)
{
}

/**
	@brief PropertiesInterface::~PropertiesInterface
*/
PropertiesInterface::~PropertiesInterface()
{
}

void PropertiesInterface::setTagName(const QString& tagname)
{
    mTagName = tagname;
}

QString PropertiesInterface::tagName() const
{
    return mTagName;
}

QStringList PropertiesInterface::supportedDatatypes() {
    // Uuid makes no sense to support from external
    return {stringS, integerS, doubleS, boolS, colorS};
}

 QString PropertiesInterface::QVariantTypeToString(const QVariant& value)
{
    switch(value.type()) {
    case QVariant::Type::String:
        return stringS;
    case QVariant::Type::Int:
        return integerS;
    case QVariant::Type::Double:
        return doubleS;
    case QVariant::Type::Bool:
        return boolS;
    case QVariant::Type::Color:
        return colorS;
    default:
        break;
    }
    return stringS; // String is always possible
}

QDomElement PropertiesInterface::toXml (QDomDocument &xml_document) const
{
    QDomElement element = xml_document.createElement(mTagName);
    toXmlPriv(element);
    propertiesToXml(element);

    return element;
}

bool PropertiesInterface::fromXml (const QDomElement &xml_element)
{
    if (!fromXmlPriv(xml_element))
        return false;

    if (!propertiesFromXml(xml_element))
        return false;

    return true;
}

bool PropertiesInterface::valideXml(QDomElement& element) {
	qDebug(QString("ValideXml() is not implemented. File: %1, Line: %2").arg(__FILE__).arg(__LINE__).toStdString().data());
	return false;
}

/**
	Permet de convertir une chaine de caracteres ("n", "s", "e" ou "w")
	en orientation. Si la chaine fait plusieurs caracteres, seul le
	premier est pris en compte. En cas d'incoherence, Qet::North est
	retourne.
	@param s Chaine de caractere cense representer une orientation
	@return l'orientation designee par la chaine de caractere
*/
Qet::Orientation PropertiesInterface::orientationFromString(const QString &s) {
	QChar c = s[0];
	// in some cases/ old projects? (affuteuse_250h.qet) numbers instead of characters are
	// used for the orientation
	if (c == 'e' || c == '1') return(Qet::East);
	else if (c == 's' || c == '2') return(Qet::South);
	else if (c == 'w' || c == '3') return (Qet::West);
	else return(Qet::North); // c == '0'
}

/**
	@param o une orientation
	@return une chaine de caractere representant l'orientation
*/
QString PropertiesInterface::orientationToString(Qet::Orientation o) {
	QString ret;
	switch(o) {
		case Qet::North: ret = "n"; break;
		case Qet::East : ret = "e"; break;
		case Qet::South: ret = "s"; break;
		case Qet::West : ret = "w"; break;
	}
	return(ret);
}

void PropertiesInterface::deleteUserProperties()
{
    properties.clear();
}

void PropertiesInterface::removeProperty(const QString& name)
{
    properties.remove(name);
}

int PropertiesInterface::userPropertiesCount() const
{
    return properties.count();
}

void PropertiesInterface::setUserProperty(const QString& key, const QVariant& value)
{
    properties[key] = value;
}

bool PropertiesInterface::existUserProperty(const QString& key) const
{
    return properties.contains(key);
}

QVariant PropertiesInterface::userProperty(const QString& key) const
{
    if (!existUserProperty(key))
        return QVariant();

    return properties[key];
}

const QHashIterator<QString, QVariant> PropertiesInterface::userPropertiesIterator() const
{
    return QHashIterator<QString, QVariant>(properties);
}

const QHash<QString, QVariant>& PropertiesInterface::userPropertiesHash() const
{
    return properties;
}

void PropertiesInterface::propertiesToXml(QDomElement& e) const
{    
    if (properties.count() == 0)
        return;

    QDomDocument doc = e.ownerDocument();
    auto up = doc.createElement(userPropertiesS);
    for (auto i = properties.begin(); i != properties.end(); ++i)
    {
        auto type = i.value().type();
        switch(type) {
        case QVariant::Type::String:
            up.appendChild(QETXML::createXmlProperty(i.key(), i.value().toString())); break;
        case QVariant::Type::Int:
            up.appendChild(QETXML::createXmlProperty(i.key(), i.value().toInt())); break;
        case QVariant::Type::Double:
            up.appendChild(QETXML::createXmlProperty(i.key(), i.value().toDouble())); break;
        case QVariant::Type::Bool:
            up.appendChild(QETXML::createXmlProperty(i.key(), i.value().toBool())); break;
        case QVariant::Type::Color:
            up.appendChild(QETXML::createXmlProperty(i.key(), QColor(i.value().value<QColor>()))); break;
        default:
            break;
        }
    }
    e.appendChild(up);
}

/*!
 * \brief PropertiesInterface::propertiesFromXml
 * Read all user properties from the DomElement& e
 * \param e
 * \return
 */
bool PropertiesInterface::propertiesFromXml(const QDomElement& e)
{
    QDomNodeList l = e.childNodes();
    for (int i=0; i < l.count(); i++)
    {
        QDomElement userProperties = l.at(i).toElement();
        if (userProperties.tagName() != userPropertiesS)
            continue;

        QDomElement userProperty;
        for (int up_index = 0; up_index < userProperties.childNodes().length(); up_index++)
        {
            userProperty = userProperties.childNodes().at(up_index).toElement();

            QString name = userProperty.attribute("name");
            QString type = userProperty.attribute("type");
            QString value = userProperty.attribute("value");

            if (type == QETXML::integerS)
            {
                int i;
                if (QETXML::propertyInteger(value, &i) == QETXML::PropertyFlags::Success)
                    properties[name] = QVariant(i);
                else
                    return false;
            }
            else if (type == QETXML::doubleS)
            {
                double d;
                if (QETXML::propertyDouble(value, &d) == QETXML::PropertyFlags::Success)
                    properties[name] = QVariant(d);
                else
                    return false;
            }
            else if (type == QETXML::boolS)
            {
                bool b;
                if (QETXML::propertyBool(value, &b) == QETXML::PropertyFlags::Success)
                    properties[name] = QVariant(b);
                else
                    return false;
            }
            else if (type == QETXML::uuidS)
            {
                QUuid u;
                if (QETXML::propertyUuid(value, &u) == QETXML::PropertyFlags::Success)
                    properties[name] = QVariant(u);
                else
                    return false;
            }
            else if (type == QETXML::colorS)
            {
                QColor c;
                if (QETXML::propertyColor(value, &c) == QETXML::PropertyFlags::Success)
                    properties[name] = QVariant(c);
                else
                    return false;
            }
            else if (type == QETXML::stringS)
            {
                properties[name] = QVariant(value);
            }
            else
            {
                qDebug() << "Not a valid property type!";
            }
        }
    }

    return true;
}

QStringList PropertiesInterface::translateSupportedDatatypes()
{
    QStringList translated;
    for (const auto &s: supportedDatatypes()) {
        QString trd;
        if (s == PropertiesInterface::integerS)
            trd = QObject::tr("Integer");
        else if (s == PropertiesInterface::doubleS)
            trd = QObject::tr("Double");
        else if (s == PropertiesInterface::boolS)
            trd = QObject::tr("Boolean");
        else if (s == PropertiesInterface::stringS)
            trd = QObject::tr("String");
        else if (s == PropertiesInterface::colorS)
            trd = QObject::tr("Color");
        else {
            assert(false); // should never occur. Then it must be implemented
            trd = "";
        }

        if (!trd.isEmpty())
            translated.append(trd);
    }
    return translated;
}

QString PropertiesInterface::translationToDatatype(const QString& name)
{
    if (name == QObject::tr("Integer"))
        return PropertiesInterface::integerS;
    else if (name == QObject::tr("Double"))
        return PropertiesInterface::doubleS;
    else if (name == QObject::tr("Boolean"))
        return PropertiesInterface::boolS;
    else if (name == QObject::tr("String"))
        return PropertiesInterface::stringS;
    else if (name == QObject::tr("Color"))
        return PropertiesInterface::colorS;

    return "";
}
