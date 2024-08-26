#ifndef VIEWER_H
#define VIEWER_H

#include <qmainwindow.h>
#include <qscrollarea.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <QKeyEvent>
#include "PDFEngine.h"

class Viewer : public QMainWindow
{
	Q_OBJECT
public:
	explicit Viewer(QWidget* parent = 0);
	~Viewer();
	void keyPressEvent(QKeyEvent* event);
private:
	PDFEngine* engine;
	QScrollArea* scrollArea;
	QLabel* totalPage;
	QLineEdit* pageNumber;
	QPushButton* upButton;
	QPushButton* downButton;
	QComboBox* scaleBox;
	QPushButton* backwardsSearch;
	QPushButton* forwardsSearch;
	QLineEdit* searchBox;
	void setAndUpdatePageKey(int key = -1);
private slots:
	void openFile();
	void exitApp();
	void aboutApp();
	void setAndUpdatePage();
	void setAndUpdateScale();
	void findPhrase();

};

#endif
