#include "iprocessingitem.h"

IProcessingItem::IProcessingItem(QObject *parent) : QObject(parent)
{
    this->m_enabled = true;
    this->m_next_item = NULL;
}

IProcessingItem::~IProcessingItem()
{

}

void IProcessingItem::setNextItem(IProcessingItem *item)
{
    this->m_next_item = item;
}

void IProcessingItem::setEnabled(bool enabled)
{
    this->m_enabled = enabled;
}

bool IProcessingItem::isEnabled()
{
    return this->m_enabled;
}
