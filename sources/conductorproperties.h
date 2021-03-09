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
#ifndef CONDUCTOR_PROPERTIES_H
#define CONDUCTOR_PROPERTIES_H

#include "qet.h"
#include <QColor>
#include <QSettings>

#include "properties/propertiesinterface.h"

class QPainter;

/**
	@brief The SingleLineProperties class
	This class represents the properties of a singleline conductor.
*/
class SingleLineProperties: public PropertiesInterface {
	public:
	SingleLineProperties();
	virtual ~SingleLineProperties();

	void setPhasesCount(int);
	unsigned short int phasesCount();
	bool isPen() const;
	void draw(QPainter *, QET::ConductorSegmentType, const QRectF &);
    void toXmlPriv(QDomElement&) const override;
    bool fromXmlPriv(const QDomElement &) override;
	static bool valideXml(QDomElement& element);
	void toSettings(QSettings &, const QString & = QString()) const;
	void fromSettings(QSettings &, const QString & = QString());

	/// Whether the singleline conductor should display the ground symbol
	bool hasGround{true};
	/// Whether the singleline conductor should display the neutral symbol
	bool hasNeutral{true};
	/// Protective Earth Neutral: visually merge neutral and ground
	bool is_pen{false};

	int operator==(const SingleLineProperties &) const;
	int operator!=(const SingleLineProperties &) const;

	private:
	unsigned short int phases{1};
	void drawGround (QPainter *, QET::ConductorSegmentType, QPointF, qreal);
	void drawNeutral(QPainter *, QPointF, qreal);
	void drawPen(QPainter *, QET::ConductorSegmentType, QPointF, qreal);
};

/**
	@brief The ConductorProperties class
	This class represents the functional properties of a particular conductor,
	i.e. properties other than path and terminals.
*/
class ConductorProperties: public PropertiesInterface
{
	public:
		ConductorProperties();
		virtual ~ConductorProperties();

			/**
				@brief The ConductorType enum Represents
				the kind of a particular conductor:
				Single: singleline symbols, no text input
				Multi: text input, no symbol
			*/
		enum ConductorType { Single, Multi };


		//Attributes
        ConductorType type{ConductorType::Multi};

		// TODO: set default values!
		QColor		color{QColor(Qt::black)},
				m_color_2{QColor(Qt::black)},
				text_color{QColor(Qt::black)};

		QString
        text{"_"},
		m_function,
		m_tension_protocol,
		m_wire_color,
		m_wire_section,
		m_formula,
		m_bus,
		m_cable;

        int text_size{9},
		m_dash_size = 1;

            double
        cond_size{1},
            verti_rotate_text{270},
            horiz_rotate_text{0};

		bool	m_show_text{true},
                m_one_text_per_folio{false},
		m_bicolor = false;

		Qt::Alignment 
		m_horizontal_alignment = Qt::AlignBottom,        
		m_vertical_alignment = Qt::AlignRight;

		Qt::PenStyle style{Qt::PenStyle::SolidLine};

		SingleLineProperties singleLineProperties;

		// methods
		static bool valideXml(QDomElement& element);
        static QString xmlTagName();
        void toSettings(QSettings &, const QString & = QString()) const override;
        void fromSettings(QSettings &, const QString & = QString()) override;
		static QString typeToString(ConductorType);
		void applyForEqualAttributes(QList<ConductorProperties> list);

		static ConductorProperties defaultProperties();

		// operators
		bool operator==(const ConductorProperties &) const;
		bool operator!=(const ConductorProperties &) const;

    private:
        void toXmlPriv(QDomElement&) const override;
        bool fromXmlPriv(const QDomElement &) override;

	private:
		void readStyle(const QString &);
		QString writeStyle() const;
};

Q_DECLARE_METATYPE(ConductorProperties)

#endif
