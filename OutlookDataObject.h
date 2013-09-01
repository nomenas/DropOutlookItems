#pragma once

#include <QStringList>

class QMimeData;

class OutlookDataObject
{
public:
    OutlookDataObject(const QMimeData* data);
    ~OutlookDataObject(void);

    bool isValid() const;
    QStringList filenames() const;
    QByteArray fileContent(const QString& filename) const;

private:
    const QMimeData* m_data;
};

