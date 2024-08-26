#ifndef VIEWER_H
#define VIEWER_H

#include <qmainwindow.h>
#include <qscrollarea.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include "PDFEngine.h"

class Viewer : public QMainWindow
{
	Q_OBJECT
public:
	explicit Viewer(QWidget* parent = 0);
	~Viewer();
private:
	PDFEngine* engine;
	QScrollArea* scrollArea;
	QLabel* totalPage;
	QLineEdit* pageNumber;
	QPushButton* upButton;
	QPushButton* downButton;
	QComboBox* scaleBox;
private slots:
	void openFile();
	void exitApp();
	void aboutApp();
	void setAndUpdatePage();
	void setAndUpdateScale();

};

#endif
