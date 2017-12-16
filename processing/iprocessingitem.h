#ifndef IPROCESSINGITEM_H
#define IPROCESSINGITEM_H

#include <QObject>
#include "processingresult.h"

class IProcessingItem : public QObject
{
    Q_OBJECT
public:
    explicit IProcessingItem(QObject *parent = nullptr);
    ~IProcessingItem();

    void setNextItem(IProcessingItem* item);
    void setEnabled(bool enabled);

    virtual void process(ProcessingResult result)
    {
       if(m_next_item!=NULL)
       {
            m_next_item->process(result);
       }
    }

    bool isEnabled();

protected:
    bool m_enabled;

private:
    IProcessingItem* m_next_item;
};

#endif // IPROCESSINGITEM_H
