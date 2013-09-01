#ifndef DROPOUTLOOKITEM_H
#define DROPOUTLOOKITEM_H

#include <QtGui/QMainWindow>
#include "ui_dropoutlookitem.h"

class DropOutlookItem : public QWidget
{
    Q_OBJECT

public:
    DropOutlookItem(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~DropOutlookItem();

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void leaveEvent(QEvent* event);

private:
    void updateMessage(QEvent* event);
    Ui::DropOutlookItemClass ui;
};

#endif // DROPOUTLOOKITEM_H
