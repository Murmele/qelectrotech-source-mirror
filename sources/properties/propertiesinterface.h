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
#ifndef PROPERTIESINTERFACE_H
#define PROPERTIESINTERFACE_H

#include <QString>
#include <QSettings>
#include <QColor>
#include <QDomElement>
#include <limits>
#include "sources/qet.h"
#include <QUuid>

/**
	@brief The PropertiesInterface class
	This class is an interface for have common way
	to use properties in QElectroTech
*/
class PropertiesInterface
{
	public:
    PropertiesInterface(const QString& tagname = "Properties");
		virtual ~PropertiesInterface();
		/**
			@brief toSettings
			Save properties to setting file.
			@param settings : is use for prefix a word
			befor the name of each paramètre
			@param QString
		*/
        virtual void toSettings(QSettings &,
                                const QString & = QString()) const =0;
		/**
			@brief fromSettings
			load properties to setting file.
			@param settings : is use for prefix a word
			befor the name of each paramètre
			@param QString
		*/
        virtual void fromSettings(QSettings &,
                                  const QString & = QString()) = 0;
		/**
			@brief toXml
			Save properties to xml element
			@param xml_document
			@return QDomElement
		*/
        virtual QDomElement toXml (QDomDocument &xml_document) const;
		/**
			@brief fromXml
			load properties to xml element
			@param xml_element
			@return true / false
		*/
        virtual bool fromXml (const QDomElement &xml_element);

    /*!
     * \brief deleteUserProperties
     * Delete all userproperties
     */
    void deleteUserProperties();

    /*!
     * \brief userPropertiesCount
     * Returns the number of user properties
     * \return
     */
    int userPropertiesCount() const;

    /*!
     * \brief existUserProperty
     * Checks if a user property with key \p key is available or not
     * \param key
     * \return
     */
    bool existUserProperty(const QString& key) const;

    /*!
     * \brief userProperty
     * Returns the value of a user property with key \p key
     * If \p key is not found, an invalid QVariant is returned.
     * Use QVariant::type() to get the type of the vale
     * \param key
     * \return
     */
    QVariant userProperty(const QString& key) const;

    /*!
     * \brief userProperties
     * Get iterator to the user Properties
     * Use
     * QHashIterator<int, QWidget *> i(hash);
     *   while (i.findNext(widget)) {
     *       qDebug() << "Found widget " << widget << " under key "
     *                << i.key();
     *   }
     * to iterate over the properties
     * \return
     */
    const QHash<QString, QVariant>* userProperties() const;

	static bool valideXml(QDomElement& element);

    static QStringList supportedDatatypes() {return {"String", "Integer", "Double", "Boolean", "Color"};} // Uuid makes no sense to support
    static QString QVariantTypeToString(const QVariant& value);
	/*!
	 * Use this functions to add properties to the xml document
	 */
	static QDomElement createXmlProperty(const QString& name, const QString value);
    static QDomElement createXmlProperty(const QString& name, const char* value);
	static QDomElement createXmlProperty(const QString& name, const int value);
	static QDomElement createXmlProperty(const QString& name, const double value);
	static QDomElement createXmlProperty(const QString& name, const bool value);
	static QDomElement createXmlProperty(const QString& name, const QUuid value);
	static QDomElement createXmlProperty(const QString& name, const QColor value);

	static QDomElement property(const QDomElement& e, const QString& name);
	static bool attribute(const QDomElement& e, const QString& attribute_name, const QString& type, QString* attr);

	enum PropertyFlags {
		Success = 0,
		NotFound = 1,
		NoValidConversion = 2,
		// = 4
	};

    static PropertyFlags propertyInteger(const QString& value, int* entry = nullptr);
	static PropertyFlags propertyInteger(const QDomElement &e, const QString& attribute_name, int *entier = nullptr);
    static PropertyFlags propertyDouble(const QString& value, double* entry = nullptr);
	static PropertyFlags propertyDouble(const QDomElement &e, const QString& attribute_name, double *reel = nullptr);
	static PropertyFlags propertyString(const QDomElement& e, const QString& attribute_name, QString* string = nullptr);
    static PropertyFlags propertyBool(const QString& value, bool* entry = nullptr);
	static PropertyFlags propertyBool(const QDomElement &e, const QString& attribute_name, bool* boolean = nullptr);
    static PropertyFlags propertyUuid(const QString& value, QUuid* entry = nullptr);
	static PropertyFlags propertyUuid(const QDomElement &e, const QString& attribute_name, QUuid* uuid = nullptr);
    static PropertyFlags propertyColor(const QString& value, QColor* entry = nullptr);
	static PropertyFlags propertyColor(const QDomElement &e, const QString& attribute_name, QColor* color = nullptr);


	static bool validXmlProperty(const QDomElement& e);

	QVariant XmlProperty(const QDomElement& element);
    void setTagName(const QString& tagname);
    QString tagName() const;

	/**
		Permet de convertir une chaine de caracteres ("n", "s", "e" ou "w")
		en orientation. Si la chaine fait plusieurs caracteres, seul le
		premier est pris en compte. En cas d'incoherence, Qet::North est
		retourne.
		@param s Chaine de caractere cense representer une orientation
		@return l'orientation designee par la chaine de caractere
	*/
	static Qet::Orientation orientationFromString(const QString &s);

	/**
		@param o une orientation
		@return une chaine de caractere representant l'orientation
	*/
	static QString orientationToString(Qet::Orientation o);

protected:
    /*!
     * \brief setUserProperty
     * Adds a new property if \p key does not exist in the \p properties member,
     * otherwise overwrite the value
     *
     * This function is protected, because otherwise this function gets called from external and
     * no change signal is emitted, which is implemented in customElementGraphicPart
     * \param key
     * \param value
     */
    void setUserProperty(const QString& key, const QVariant& value);

private:
    virtual void toXmlPriv (QDomElement &e) const =0;
    virtual bool fromXmlPriv (const QDomElement &e) =0;
    /*!
     * \brief PropertiesInterface::propertiesToXml
     * Write all user properties to the DomElement \p e
     * \param e
     */
    void propertiesToXml(QDomElement& e) const;
    bool propertiesFromXml (const QDomElement &e);

    QHash<QString, QVariant> properties;
    QString mTagName{""};
};

#endif // PROPERTIESINTERFACE_H
