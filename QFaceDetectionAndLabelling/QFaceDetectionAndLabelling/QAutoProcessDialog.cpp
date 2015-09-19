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


#include <QtWidgets/QMessageBox>
#include "QAutoProcessDialog.h"

/**
	@author: Catree
	@date: 2015/09/19
**/


QAutoProcessDialog::QAutoProcessDialog(DetectAndLabelizeFaces &detectEngine, const std::vector<std::string> &vectorOfImageFilepaths, const std::string &outputDir, const bool overwrite, QWidget *parent, Qt::WindowFlags f) 
	: QDialog(parent, f), m_detectEngine(&detectEngine), m_outputDirectory(outputDir)
{
	m_threadDetectEngine = new QAutoProcessThread(&detectEngine, vectorOfImageFilepaths, outputDir, overwrite, this);

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
	connect(m_threadDetectEngine, SIGNAL(sendError()), this, SLOT(updateError()));

	//Start thread
	m_threadDetectEngine->start();

	show();
}

QAutoProcessDialog::~QAutoProcessDialog()
{
	if(m_threadDetectEngine != NULL)
	{
		delete m_threadDetectEngine;
	}
}

void QAutoProcessDialog::updateTextEdit(const std::string &text)
{
	m_textEdit->append(QString::fromStdString(text));
}

void QAutoProcessDialog::updateProgressBar(const int value)
{
	m_progressBar->setValue(value);
}

void QAutoProcessDialog::updateProgressLabel(const std::string &label)
{
	m_progressLabel->setText(QString::fromStdString(label));
}

void QAutoProcessDialog::updateError()
{
	QMessageBox::warning(this, tr("Error"), tr("Problem with Face detection engine !"));
	QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
}

void QAutoProcessDialog::closeEvent(QCloseEvent *event)
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
