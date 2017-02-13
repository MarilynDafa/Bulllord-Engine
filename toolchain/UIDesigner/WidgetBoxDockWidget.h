#ifndef WIDGETBOXDOCK_H
#define WIDGETBOXDOCK_H

#include "GeneratedFiles/ui_widgetboxdockwidget.h"

class WidgetBoxDockWidget : public QWidget , public Ui::WidgetBoxDockWidget
{
	Q_OBJECT
public:
	WidgetBoxDockWidget(QWidget *parent = 0);
private:
	void initUI();
};

#endif //WIDGETBOXDOCK_H