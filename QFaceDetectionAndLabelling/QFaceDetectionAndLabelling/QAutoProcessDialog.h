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


#ifndef QAUTOPROCESSDIALOG_H
#define QAUTOPROCESSDIALOG_H

#include <iostream>
#include <sstream>

#include <QtWidgets/QDialog>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>

#include "DetectAndLabelizeFaces.h"
#include "QAutoProcessThread.h"

/**
	@author: Catree
	@date: 2015/09/19
**/


class QAutoProcessDialog : public QDialog
{
	Q_OBJECT

private:
	QTextEdit *m_textEdit;
	QProgressBar *m_progressBar;
	QLabel *m_progressLabel;
	DetectAndLabelizeFaces *m_detectEngine;
	std::string m_outputDirectory;
	
	void closeEvent(QCloseEvent *event);
        
public:
	QAutoProcessThread *m_threadDetectEngine;

    QAutoProcessDialog(DetectAndLabelizeFaces &detectEngine, const std::vector<std::string> &vectorOfImageFilepaths, const std::string &outputDir, const bool overwrite, QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~QAutoProcessDialog(void);

private slots:
	void updateTextEdit(const std::string &text);
	void updateProgressBar(const int value);
	void updateProgressLabel(const std::string &label);
	void updateError();
}; 

#endif
