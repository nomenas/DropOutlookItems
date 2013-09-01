#include "OutlookDataObject.h"
#include "qdnd_p.h"

#include <QWindowsMime>
#include <QMimeData>
#include <Shlobj.h>
#include <ObjIdl.h>

OutlookDataObject::OutlookDataObject(const QMimeData* data)
    : m_data(data)
{
}

OutlookDataObject::~OutlookDataObject(void)
{
}

bool OutlookDataObject::isValid() const
{
    return m_data && m_data->hasFormat("FileGroupDescriptorW");

}

QStringList OutlookDataObject::filenames() const
{
    QStringList returnValue;
    
    if (m_data)
    {
        QByteArray fileGroupDescriptorBytes = m_data->data("FileGroupDescriptorW");
        FILEGROUPDESCRIPTORW* fileDescriptor = (FILEGROUPDESCRIPTORW*) fileGroupDescriptorBytes.data();
        
        for (int i = 0; i < fileDescriptor->cItems; ++i)
        {
            returnValue << QString::fromWCharArray(fileDescriptor->fgd[i].cFileName);
        }
    }
    
    return returnValue;
}

QByteArray OutlookDataObject::fileContent(const QString& filename) const
{
    QByteArray returnValue;
    
    if (const QDropData* dropData = dynamic_cast<const QDropData*>(m_data))
    {
        int indexOfStream = filenames().indexOf(filename);
        if (indexOfStream >= 0)
        {
            FORMATETC formatetc;
            formatetc.cfFormat = 49325;
            formatetc.ptd = 0;
            formatetc.dwAspect = 1;
            formatetc.lindex = 0;
            formatetc.tymed = 12;
            
            STGMEDIUM medium = {0};
            if (dropData->currentDataObject->GetData(&formatetc, &medium) == S_OK)
            {
                if (medium.tymed == TYMED_ISTREAM)
                {
                    STATSTG iStreamStat = {0};
                    medium.pstm->Stat(&iStreamStat, 0);
                    returnValue.resize(static_cast<int>(iStreamStat.cbSize.QuadPart));
                    ULONG pcbRead = 0;
                    medium.pstm->Read((void*) returnValue.constData(), returnValue.size(), &pcbRead);
                }
                else if (medium.tymed == TYMED_ISTORAGE)
                {
                    //create a ILockBytes (unmanaged byte array) and then create a IStorage using the byte array as a backing store
                    ILockBytes* iLockBytes = 0;
                    HRESULT res = CreateILockBytesOnHGlobal(0, true, &iLockBytes);
                    IStorage* iStorage = 0;
                    res = StgCreateDocfileOnILockBytes(iLockBytes, 0x00001012, 0, &iStorage);

                    //copy the returned IStorage into the new IStorage
                    medium.pstg->CopyTo(0, 0, 0, iStorage);
                    iLockBytes->Flush();
                    iStorage->Commit(0);

                    //get the STATSTG of the ILockBytes to determine how many bytes were written to it
                    STATSTG iLockBytesStat = {0};
                    iLockBytes->Stat(&iLockBytesStat, 1);
                    returnValue.resize(iLockBytesStat.cbSize.QuadPart);

                    ULARGE_INTEGER offset;
                    offset.QuadPart = 0;
                    iLockBytes->ReadAt(offset, (void*) returnValue.constData(), returnValue.size(), 0);

                    iStorage->Release();
                    iLockBytes->Release();
                }
            }
        }
    }
    
    return returnValue;
}
