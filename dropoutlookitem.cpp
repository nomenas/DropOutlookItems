#include "dropoutlookitem.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDebug>
#include "OutlookDataObject.h"

DropOutlookItem::DropOutlookItem(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags)
{
    ui.setupUi(this);
    setAcceptDrops(true);
    updateMessage(0);
}

DropOutlookItem::~DropOutlookItem()
{

}

void DropOutlookItem::dragEnterEvent(QDragEnterEvent* event)
{
    if (OutlookDataObject(event->mimeData()).isValid())
    {
        updateMessage(event);
        event->acceptProposedAction();
    }
}

void DropOutlookItem::dragLeaveEvent(QDragLeaveEvent* event)
{
    updateMessage(event);
    QWidget::dragLeaveEvent(event);
}

void DropOutlookItem::dropEvent(QDropEvent* event)
{
    OutlookDataObject outlookDataObject(event->mimeData());
    QStringList filenames = outlookDataObject.filenames();
    
    foreach (QString filename, filenames)
    {
        QByteArray fileContent = outlookDataObject.fileContent(filename);
        QFile file(QApplication::applicationDirPath() + "\\" + filename);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(fileContent.constData(), fileContent.size());
            file.close();
        }
    }

    updateMessage(event);
    QWidget::dropEvent(event);
}

void DropOutlookItem::leaveEvent(QEvent* event)
{
    updateMessage(event);
    QWidget::leaveEvent(0);
}

void DropOutlookItem::updateMessage(QEvent* event)
{
    if (QDragEnterEvent* dragEvent = dynamic_cast<QDragEnterEvent*>(event))
    {
        ui.lblMessage->setText(dragEvent->mimeData()->text()); 
    }
    else if (QDropEvent* dropEvent = dynamic_cast<QDropEvent*>(event))
    {
        OutlookDataObject outlookDataObject(dropEvent->mimeData());
        QString message = "Created the following files:\n";
        foreach (QString filename, outlookDataObject.filenames())
        {
            message += filename + "\n";
        }

        ui.lblMessage->setText(message); 
    }
    else 
    {
        ui.lblMessage->setText("Drop Outlook item to save message to application path");
    }
}
