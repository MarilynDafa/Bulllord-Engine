#include "WidgetBoxDockWidget.h"


WidgetBoxDockWidget::WidgetBoxDockWidget( QWidget *parent /*= 0*/ )
	: QWidget(parent)
{
	initUI();
}

void WidgetBoxDockWidget::initUI()
{
	setupUi(this);
}