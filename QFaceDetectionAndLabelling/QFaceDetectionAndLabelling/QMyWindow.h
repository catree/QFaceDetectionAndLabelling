/**
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
**/


#ifndef QMYWINDOW_H
#define QMYWINDOW_H

#include <iostream>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "QOpenCVWidget.h"
#include "DetectAndLabelizeFaces.h"
#include "QProcessingDialog.h"

class QOpenCVWidget;

/**
	@author: Catree
	@date: 2015/07/14
**/


class QMyWindow : public QMainWindow
{
	Q_OBJECT

public:
	QMyWindow(void);
	~QMyWindow(void);

	void deleteCurrentFace(const int posX, const int poxY);
	void addCurrentFace(const int posX, const int poxY, const int labelNumber);

private slots:
    void open();
    void save();
	void help();
	void about();
    void aboutQt();
	void previous();
	void next();
	void updateProgressFinished();

private:
	QOpenCVWidget* m_imageWidget;
	QMenu *m_fileMenu;
	QMenu *m_helpMenu;

	QAction *m_openAct;
	QAction *m_saveAct;
	QAction *m_exitAct;
	QAction *m_helpAct;
	QAction *m_aboutAct;
    QAction *m_aboutQtAct;
	
	QLabel *m_imageLabel;
	QPushButton *previousButton;
	QPushButton *nextButton;

	DetectAndLabelizeFaces m_detectEngine;
	std::vector<std::string> m_vectorOfImageFilepaths;
	int m_currentPosition;

	QProcessingDialog *m_processingDialog;

	void createActions();
	void createMenus();
};

#endif
