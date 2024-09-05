#ifndef VIEWER_H
#define VIEWER_H

#include <qmainwindow.h>
#include <qscrollarea.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <vector>
#include <QKeyEvent>
#include "PDFEngine.h"
#include "NavigationBar.h"
#include "TabItem.h"

class Viewer : public QMainWindow
{
	Q_OBJECT
public:
	explicit Viewer(QWidget* parent = 0);
	~Viewer();
	void keyPressEvent(QKeyEvent* event);
private:
	QLabel* totalPage;
	QLineEdit* pageNumber;
	QPushButton* upButton;
	QPushButton* downButton;
	QComboBox* scaleBox;
	QPushButton* backwardsSearch;
	QPushButton* forwardsSearch;
	QLineEdit* searchBox;
	QHBoxLayout* layout;
	QAction* navBarShowAct;
	NavigationBar* navBar;
	QAction* rotate90CWAct, *rotate90CCWAct;
	QTabWidget* tWidget;
	std::vector <TabItem*> tabItems;
	int currentTab;
	void setAndUpdatePageKey(int key = -1);
private slots:
	void openFile();
	void exitApp();
	void aboutApp();
	void setAndUpdatePage();
	void setAndUpdateScale();
	void findPhrase();
	void getPageText();
	void showNavBar();
	void updatePageNavBar(const int pNum);
	void rotatePage();
	void onTabClicked(int index);
	void onTabMoved(int from, int to);
	void onTabCloseRequested(int index);
	void getPrintDialog();
};

#endif
