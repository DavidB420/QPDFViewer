#ifndef VIEWER_H
#define VIEWER_H

#include <qmainwindow.h>

class Viewer : public QMainWindow
{
	Q_OBJECT
public:
	explicit Viewer(QWidget* parent = 0);
	~Viewer();
private slots:
	void openFile();
	void exitApp();
	void aboutApp();
};

#endif
