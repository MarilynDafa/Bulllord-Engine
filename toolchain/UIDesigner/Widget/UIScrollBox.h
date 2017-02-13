#ifndef UI_SCROLLBOX_H
#define UI_SCROLLBOX_H

#include "UiWidget.h"

using namespace o2d;

class UiScrollBox : public UiWidget
{
	Q_OBJECT
	Q_CLASSINFO("uiClassName", "c_scrollbox")
	Q_PROPERTY(bool strict READ getStrict WRITE setStrict)
public:
	bool getStrict() const;
	void setStrict(bool val);
public:
	Q_INVOKABLE UiScrollBox(UiWidget *parent = 0);
	virtual bool acceptChildern() const { return true; }
	virtual void getDefaultSize(int &width, int &height) const;
	virtual QString getUiClassName() const { return "c_scrollbox"; }
	virtual QDomElement serialize( QDomDocument &doc, const QDomElement &extElement);
	virtual void readExt(QXmlStreamReader &reader);
protected Q_SLOTS:
	virtual void slotSkinFileChanged(const SkinFile &skinFile);
};

#endif //UILABEL_H