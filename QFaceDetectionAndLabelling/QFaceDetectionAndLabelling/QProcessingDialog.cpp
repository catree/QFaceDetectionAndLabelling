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


#include "QProcessingDialog.h"

/**
	@author: Catree
	@date: 2015/07/14
**/


QProcessingDialog::QProcessingDialog(DetectAndLabelizeFaces &detectEngine, const std::vector<std::string> &vectorOfImageFilepaths, QWidget *parent, Qt::WindowFlags f) 
	: QDialog(parent, f)
{
	m_threadDetectEngine = new QThreadFaceDetectionAndLabelling(&detectEngine, vectorOfImageFilepaths, this);

	QGridLayout *gridLayout = new QGridLayout;
	m_textEdit = new QTextEdit;
	m_textEdit->setDisabled(true);
	gridLayout->addWidget(m_textEdit, 0, 0, 4, 4);

	QHBoxLayout *hBoxLayout = new QHBoxLayout;

	m_progressBar = new QProgressBar;
	hBoxLayout->addWidget(m_progressBar);

	std::stringstream ss;
	ss << "0/" << vectorOfImageFilepaths.size();
	m_progressLabel = new QLabel(QString::fromStdString(ss.str()));
	hBoxLayout->addWidget(m_progressLabel);
	
	gridLayout->addLayout(hBoxLayout, 4, 0, 1, 4);
	setLayout(gridLayout);

	setModal(true);
	setMinimumSize(320, 240);
	
	connect(m_threadDetectEngine, SIGNAL(sendTextEdit(const std::string &)), this, SLOT(updateTextEdit(const std::string &)));
	connect(m_threadDetectEngine, SIGNAL(sendProgressBar(const int)), this, SLOT(updateProgressBar(const int)));
	connect(m_threadDetectEngine, SIGNAL(sendProgressLabel(const std::string &)), this, SLOT(updateProgressLabel(const std::string &)));

	//Start thread
	m_threadDetectEngine->start();

	show();
}

QProcessingDialog::~QProcessingDialog()
{
	if(m_threadDetectEngine != NULL)
	{
		delete m_threadDetectEngine;
	}
}

void QProcessingDialog::updateTextEdit(const std::string &text)
{
	m_textEdit->append(QString::fromStdString(text));
}

void QProcessingDialog::updateProgressBar(const int value)
{
	m_progressBar->setValue(value);
}

void QProcessingDialog::updateProgressLabel(const std::string &label)
{
	m_progressLabel->setText(QString::fromStdString(label));
}

void QProcessingDialog::closeEvent(QCloseEvent *event)
{
	m_threadDetectEngine->m_stop = true;
	//@see: https://forum.qt.io/topic/20539/what-is-the-correct-way-to-stop-qthread/3
	if(!m_threadDetectEngine->wait(5000)) //Wait until it actually has terminated (max. 5 sec)
	{
		qWarning("Thread deadlock detected, bad things may happen !!!");
		m_threadDetectEngine->terminate(); //Thread didn't exit in time, probably deadlocked, terminate it!
		m_threadDetectEngine->wait(); //Note: We have to wait again here!
	}
}
