#include "userpropertyundocommand.h"

#include "graphicspart/customelementpart.h"
#include "graphicspart/customelementgraphicpart.h"
#include "graphicspart/partdynamictextfield.h"
#include "graphicspart/parttext.h"

void UserPropertyUndoCommand::addUserProperty(const QString& name, const QVariant& value)
{
    for (auto part: mParts) {

        CustomElementPart::Type type = part->elementType();

        // Problem is that PartText and PartDynamicTextEdit derive not from
        // Custom ElementGraphicPart
        if (type == CustomElementPart::Type::ElementGraphics) {
            auto g = dynamic_cast<CustomElementGraphicPart*>(part);
            g->setUserProperty(name, value);
        } else if (type == CustomElementPart::Type::Text) {

            auto t = dynamic_cast<QGraphicsTextItem*>(part);
            if (t->type() == QGraphicsItem::UserType + 1110) {
                // PartDynamicTextField
                auto pd = static_cast<PartDynamicTextField*>(t);
                pd->setUserProperty(name, value);
            }
            else if (t->type() == QGraphicsItem::UserType + 1107) {
                // PartText
                auto pt = static_cast<PartText*>(t);
                pt->setUserProperty(name, value);
            }

        } else {
            qDebug() << "ElementItemEditor::setUpChangeConnections(): Should be unreachable!";
        }
    }
}

void UserPropertyUndoCommand::removeUserProperty(const QString& name)
{
    for (auto part: mParts) {

        CustomElementPart::Type type = part->elementType();

        // Problem is that PartText and PartDynamicTextEdit derive not from
        // Custom ElementGraphicPart
        if (type == CustomElementPart::Type::ElementGraphics) {
            auto g = dynamic_cast<CustomElementGraphicPart*>(part);
            g->removeUserProperty(name);
        } else if (type == CustomElementPart::Type::Text) {

            auto t = dynamic_cast<QGraphicsTextItem*>(part);
            if (t->type() == QGraphicsItem::UserType + 1110) {
                // PartDynamicTextField
                auto pd = static_cast<PartDynamicTextField*>(t);
                pd->removeUserProperty(name);
            }
            else if (t->type() == QGraphicsItem::UserType + 1107) {
                // PartText
                auto pt = static_cast<PartText*>(t);
                pt->removeUserProperty(name);
            }

        } else {
            qDebug() << "ElementItemEditor::setUpChangeConnections(): Should be unreachable!";
        }
    }
}

void UserPropertyUndoCommand::setUserProperty(const QString& name, const QVariant& value)
{
    for (auto part: mParts) {

        CustomElementPart::Type type = part->elementType();

        // Problem is that PartText and PartDynamicTextEdit derive not from
        // Custom ElementGraphicPart
        if (type == CustomElementPart::Type::ElementGraphics) {
            auto g = dynamic_cast<CustomElementGraphicPart*>(part);
            g->setUserProperty(name, value);
        } else if (type == CustomElementPart::Type::Text) {

            auto t = dynamic_cast<QGraphicsTextItem*>(part);
            if (t->type() == QGraphicsItem::UserType + 1110) {
                // PartDynamicTextField
                auto pd = static_cast<PartDynamicTextField*>(t);
                pd->setUserProperty(name, value);
            }
            else if (t->type() == QGraphicsItem::UserType + 1107) {
                // PartText
                auto pt = static_cast<PartText*>(t);
                pt->setUserProperty(name, value);
            }

        } else {
            assert(false);
            qDebug() << "ElementItemEditor::setUpChangeConnections(): Should be unreachable!";
        }
    }
}

void UserPropertyUndoCommand::saveProperties(QList<QVariant>& values) {
    QVariant value;
    for (auto part: mParts) {

        CustomElementPart::Type type = part->elementType();

        // Problem is that PartText and PartDynamicTextEdit derive not from
        // Custom ElementGraphicPart
        if (type == CustomElementPart::Type::ElementGraphics) {
            auto g = dynamic_cast<CustomElementGraphicPart*>(part);
            value = g->userProperty(mName);
        } else if (type == CustomElementPart::Type::Text) {

            auto t = dynamic_cast<QGraphicsTextItem*>(part);
            if (t->type() == QGraphicsItem::UserType + 1110) {
                // PartDynamicTextField
                auto pd = static_cast<PartDynamicTextField*>(t);
                value = pd->userProperty(mName);
            }
            else if (t->type() == QGraphicsItem::UserType + 1107) {
                // PartText
                auto pt = static_cast<PartText*>(t);
                value = pt->userProperty(mName);
            }

        } else {
            qDebug() << "ElementItemEditor::setUpChangeConnections(): Should be unreachable!";
        }

        values.append(value);
    }
}

void UserPropertyUndoCommand::restoreProperties(const QList<QVariant>& values)
{
    CustomElementPart* part;
    for (int i=0; i < mParts.count(); i++) {
        if (values[i].isNull())
            continue;

        part = mParts[i];
        CustomElementPart::Type type = part->elementType();

        // Problem is that PartText and PartDynamicTextEdit derive not from
        // Custom ElementGraphicPart
        if (type == CustomElementPart::Type::ElementGraphics) {
            auto g = dynamic_cast<CustomElementGraphicPart*>(part);
            g->setUserProperty(mName, values[i]);
        } else if (type == CustomElementPart::Type::Text) {

            auto t = dynamic_cast<QGraphicsTextItem*>(part);
            if (t->type() == QGraphicsItem::UserType + 1110) {
                // PartDynamicTextField
                auto pd = static_cast<PartDynamicTextField*>(t);
                pd->setUserProperty(mName, values[i]);
            }
            else if (t->type() == QGraphicsItem::UserType + 1107) {
                // PartText
                auto pt = static_cast<PartText*>(t);
                pt->setUserProperty(mName, values[i]);
            }

        } else {
            qDebug() << "restoreProperties(): Should be unreachable!";
        }
    }
}
