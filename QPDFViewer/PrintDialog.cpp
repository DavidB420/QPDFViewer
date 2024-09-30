#include "PrintDialog.h"

PrintDialog::PrintDialog(QWidget* parent)
{
	//Create print dialog
	this->setWindowTitle("Print");
	this->resize(640, 480);
	this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}
