#include "FontSetDialog.h"
#include "Config.h"
#include <assert.h>

FontSetDialog::FontSetDialog( QWidget *parent /*= 0*/ )
	: QDialog(parent)
{
	initUi();
	initSignal();
}

FontSetDialog::~FontSetDialog()
{
	GlobalConfig::getSingletonPtr()->setValue("antialiased", QVariant(_antialiased));
	GlobalConfig::getSingletonPtr()->setValue("outline", QVariant(_outline));
	GlobalConfig::getSingletonPtr()->setValue("letter_digit", QVariant(_letterDigit));
	GlobalConfig::getSingletonPtr()->setValue("punctuqtion", QVariant(_punctuqtion));
	GlobalConfig::getSingletonPtr()->setValue("coderangelist", QVariant(listToString()));
}

void FontSetDialog::initUi()
{
	setupUi(this);

	_antialiased = GlobalConfig::getSingletonPtr()->value("antialiased", QVariant(true)).toBool();
	_outline = GlobalConfig::getSingletonPtr()->value("outline", QVariant(false)).toBool();
	_letterDigit = GlobalConfig::getSingletonPtr()->value("letter_digit", QVariant(true)).toBool();
	_punctuqtion = GlobalConfig::getSingletonPtr()->value("punctuqtion", QVariant(true)).toBool();
	StringToList(GlobalConfig::getSingletonPtr()->value("coderangelist", QVariant("")).toString());

	_antialiasedCheck->setChecked(_antialiased);
	_outlineCheck->setChecked(_outline);

	_codeList->item(0)->setCheckState(_letterDigit ? Qt::Checked : Qt::Unchecked);
	_codeList->item(1)->setCheckState(_punctuqtion ? Qt::Checked : Qt::Unchecked);

	QListIterator<FontCodeRange> codesIter(_codeRangeList);
	while(codesIter.hasNext())
	{
		const FontCodeRange &code = codesIter.next();
		addCodeRange(code.start, code.end);
	}
}

void FontSetDialog::initSignal()
{
	connect(_addBtn, SIGNAL(clicked()), this, SLOT(slotAddBtnClicked()));
	connect(_okBtn, SIGNAL(clicked()), this, SLOT(slotOkBtnClicked()));
}

void FontSetDialog::slotAddBtnClicked()
{
	int start = _startSpin->value();
	int end = _endSpin->value();
	if(start > end)
	{
		int tmp = end;
		end = start;
		start = tmp;
	}

	addCodeRange(start, end);
}

void FontSetDialog::slotOkBtnClicked()
{
	_codeRangeList.clear();

	int count = _codeList->count();
	assert(count >= 2);

	QListWidgetItem *item0 = _codeList->item(0);
	assert(item0->data(Qt::UserRole).isNull());
	_letterDigit = (item0->checkState() == Qt::Checked);

	QListWidgetItem *item1 = _codeList->item(1);
	assert(item1->data(Qt::UserRole).isNull());
	_punctuqtion = (item1->checkState() == Qt::Checked);

	for(int i = 2; i < count; i ++)
	{
		QListWidgetItem *item = _codeList->item(i);
		QVariant data = item->data(Qt::UserRole);
		assert(!data.isNull());
		if(item->checkState() == Qt::Checked)
		{
			FontCodeRange code = data.value<FontCodeRange>();
			_codeRangeList.push_back(code);
		}
	}

	_antialiased = _antialiasedCheck->isChecked();
	_outline = _outlineCheck->isChecked();

	accept();
}

void FontSetDialog::addCodeRange( int start, int end )
{
	QString text = QString("%1 ~ %2").arg(start).arg(end);
	QListWidgetItem *item = new QListWidgetItem();
	item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	item->setData(Qt::UserRole, QVariant::fromValue(FontCodeRange(start, end)));
	item->setText(text);
	item->setCheckState(Qt::Checked);
	_codeList->addItem(item);
}

QString FontSetDialog::listToString()
{
	QStringList strList;
	QListIterator<FontCodeRange> codesIter(_codeRangeList);
	while(codesIter.hasNext())
	{
		const FontCodeRange &code = codesIter.next();
		strList.append(QString("%1").arg(code.start));
		strList.append(QString("%1").arg(code.end));
	}
	return strList.join("-");
}

void FontSetDialog::StringToList( const QString &str )
{
	QString str2 = str.trimmed();
	if(str2.isEmpty())
		return;

	QStringList strs = str2.split("-");
	QStringListIterator strIter(strs);
	while(strIter.hasNext())
	{
		int start = strIter.next().toInt();
		int end = strIter.next().toInt();
		_codeRangeList.push_back(FontCodeRange(start, end));
	}
}
