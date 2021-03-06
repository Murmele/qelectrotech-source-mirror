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
#ifndef ELEMENT_ITEM_EDITOR_H
#define ELEMENT_ITEM_EDITOR_H

#include <QWidget>

class QETElementEditor;
class ElementScene;
class CustomElementPart;
class QUndoStack;
class QListWidget;

/**
	This is the base class for primitives editors within the element editor. It
	provides methods to access the editor itself, the undo stack, the edition
	scene and even a method to easily take a ChangePartCommand into account.
*/
class ElementItemEditor : public QWidget
{
		Q_OBJECT
		// constructors, destructor
	public:
		ElementItemEditor(QETElementEditor *, QWidget * = nullptr);
		~ElementItemEditor() override {}
	private:
		ElementItemEditor(const ElementItemEditor &);
	
		// methods
	public:
		virtual QETElementEditor *elementEditor() const;
		virtual ElementScene *elementScene() const;
		virtual QUndoStack &undoStack() const;

		virtual QString elementTypeName() const;
		virtual void setElementTypeName(const QString &);
		virtual void detach();

		virtual bool setPart(CustomElementPart *) = 0;
		virtual bool setParts(QList <CustomElementPart *>) {return false;}

		virtual CustomElementPart *currentPart() const = 0;
		virtual QList<CustomElementPart*> currentParts() const = 0;
        virtual void setUpChangeConnections();
        /*!
         * \brief updateForm
         * update the values of the widget
         */
        virtual void updateForm();

        QWidget* editorWidget() {return mEditorWidget;}

        /*!
         * \brief disconnectChangeConnections
         * Disconnects all change connections and deletes the connections
         */
        void disconnectChangeConnections();
    private:
        virtual void setUpChangeConnectionsPriv() = 0;
        virtual void updateFormPriv() = 0;

private slots:
        /*!
         * \brief updateUserProperties
         * update the list with the user properties
         * \param key
         */
        void updateUserProperties(const QString& key);
	
protected:
        /*!
         * \brief m_change_connections
         * Connections from the part to the editor
         */
        QList<QMetaObject::Connection> m_change_connections;
		// attributes
	private:
		QETElementEditor *element_editor{nullptr};
		QString element_type_name;

        /*!
         * On this widget all individual widgets of the different
         * editors will be placed.
         */
        QWidget* mEditorWidget{nullptr};
		QListWidget* mUserPropertiesList{nullptr};
};
#endif
