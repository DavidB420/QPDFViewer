#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <qdialog.h>
#include <qlist.h>
#include <qprinterinfo.h>
#include <qcombobox.h>
#include <qlabel.h>

class PrintDialog : public QDialog
{
	Q_OBJECT
public:
	PrintDialog(QWidget* parent = 0);
private:
	QList<QPrinterInfo> printers;
	QComboBox *pagesPullDown;
private slots:
	void handlePagePullDown();
	void exitDialog();
};

class ClickableLink : public QLabel {
	Q_OBJECT
public:
	explicit ClickableLink(const QString& text, QWidget* parent = nullptr) {
		setCursor(Qt::PointingHandCursor);
		setText(text);
		setStyleSheet("QLabel { color : blue; text-decoration: underline;}");
	}
signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent* event) override {
		emit clicked();
	}

};

#endif
