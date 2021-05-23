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
#ifndef PART_TERMINAL_H
#define PART_TERMINAL_H

#include "../../properties/terminaldata.h"
#include "customelementgraphicpart.h"

#include <QUuid>

/**
	This class represents a terminal which may be used to compose the drawing of
	an electrical element within the element editor.
*/
class PartTerminal : public CustomElementGraphicPart
{
	Q_OBJECT

	Q_PROPERTY(Qet::Orientation orientation READ orientation WRITE setOrientation)
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(TerminalData::Type terminal_type READ terminalType WRITE setTerminalType)

	public:
		// constructors, destructor
		PartTerminal(QETElementEditor *editor, QGraphicsItem *parent = nullptr);
		~PartTerminal() override;
	private:
		PartTerminal(const PartTerminal &);

	signals:
		void orientationChanged();
		void nameChanged();
		void terminalTypeChanged();

		// methods
	public:
		enum { Type = UserType + 1106 };
			/**
				Enable the use of qgraphicsitem_cast to safely cast a QGraphicsItem into a PartTerminal.
				@return the QGraphicsItem type
			*/
		int type() const override { return Type; }
		QString xmlName() const override { return(QString("terminal")); }
		bool fromXmlPriv(const QDomElement &) override;
        void toXmlPriv(QDomElement&xml_element) const override;
		static bool valideXml(QDomElement& element);
        void toSettings(QSettings &,const QString & = QString()) const override {/*TODO: implement*/}
        void fromSettings(QSettings &,const QString & = QString()) override{/*TODO: implement*/}
		void paint(
				QPainter *painter,
				const QStyleOptionGraphicsItem *,
				QWidget *) override;

		QPainterPath shape() const override;
		QPainterPath shadowShape() const override {return shape();}
		QRectF boundingRect() const override;
		bool isUseless() const override;
		QRectF sceneGeometricRect() const override;
		void startUserTransformation(const QRectF &) override;
		void handleUserTransformation(const QRectF &, const QRectF &) override;

		Qet::Orientation orientation() const {return d -> m_orientation;}
		void setOrientation(Qet::Orientation ori);

		QString name() const override { return d -> m_name; }
		void setName(QString& name);

		TerminalData::Type terminalType() const {return d->m_type;}
		void setTerminalType(TerminalData::Type type);

		void setNewUuid();

	private:
		void updateSecondPoint();
        TerminalData* d{nullptr}; // pointer to the terminal data

	private:
        QPointF m_saved_position;
};
#endif
