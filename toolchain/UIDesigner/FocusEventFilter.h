#ifndef FOCUSEVENTFILTER_H
#define FOCUSEVENTFILTER_H

#include <QtCore/QObject>
#include <QtCore/QEvent>

class FocusEventFilter : public QObject
{
	Q_OBJECT
public:
	FocusEventFilter(QObject *parent = 0)
		: QObject(parent)
	{

	}

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event)
	{
		if(event->type() == QEvent::FocusIn)
		{
			obj->blockSignals(false);
			return true;
		}
		else if(event->type() == QEvent::FocusOut)
		{
			obj->blockSignals(true);
			return true;
		}
		return QObject::eventFilter(obj, event);
	}
};

#endif //FOCUSEVENTFILTER_H