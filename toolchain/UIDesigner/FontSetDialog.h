#ifndef FONTSETDLG_H
#define FONTSETDLG_H

#include "GeneratedFiles/ui_FontSetDialog.h"

struct FontCodeRange
{
	int start;
	int end;
	FontCodeRange()
		: start(0)
		, end(0)
	{

	}

	FontCodeRange(const FontCodeRange &range)
		: start(range.start)
		, end(range.end)
	{

	}

	FontCodeRange(int start, int end)
		: start(start)
		, end(end)
	{

	}
};
Q_DECLARE_METATYPE(FontCodeRange);

typedef QList<FontCodeRange> FontCodeRangeList;
Q_DECLARE_METATYPE(FontCodeRangeList);

class FontSetDialog : public QDialog, public Ui::FontSetDialog
{
	Q_OBJECT
public:
	FontSetDialog(QWidget *parent = 0);
	~FontSetDialog();

	bool isAntialiased() { return _antialiased; }
	bool isOutline() { return _outline; }
	bool haveLetterDigit() { return _letterDigit; }
	bool havePunctuqtion() { return _punctuqtion; }
	const FontCodeRangeList &getCodeRangeList() { return _codeRangeList; }

protected slots:
	void slotAddBtnClicked();
	void slotOkBtnClicked();

private:
	void initUi();
	void initSignal();

	void addCodeRange(int start, int end);
	QString listToString();
	void StringToList(const QString &str);

private:
	FontCodeRangeList _codeRangeList;
	bool _antialiased;
	bool _outline;
	bool _letterDigit;
	bool _punctuqtion;
};

#endif //FONTSETDLG_H