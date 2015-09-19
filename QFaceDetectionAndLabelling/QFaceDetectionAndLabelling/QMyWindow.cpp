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


#include <QtWidgets/QShortcut>

#include "QMyWindow.h"

/**
	@author: Catree
	@date: 2015/07/14
	@dateUpdate: 2015/09/19
**/


//@see: http://doc.qt.io/qt-5/qtwidgets-mainwindows-menus-example.html
QMyWindow::QMyWindow(void) : QMainWindow(), m_detectEngine(), m_vectorOfImageFilepaths(), m_currentPosition(0)
{
	QGridLayout *gridLayout = new QGridLayout;

	QWidget *centralWidget  = new QWidget;
	centralWidget->setLayout(gridLayout);
	setCentralWidget(centralWidget);

	m_imageWidget = new QOpenCVWidget(this);
	gridLayout->addWidget(m_imageWidget, 0, 0, 8, 8);
	
	m_imageLabel = new QLabel;
	previousButton = new QPushButton("<");
	nextButton = new QPushButton(">");

	QHBoxLayout *hBoxButton = new QHBoxLayout;
	hBoxButton->addStretch();
	hBoxButton->addWidget(m_imageLabel);
	hBoxButton->addWidget(previousButton);
	hBoxButton->addWidget(nextButton);
	hBoxButton->addStretch();

	gridLayout->addLayout(hBoxButton, 8, 0, 1, 8);

	connect(previousButton, SIGNAL (released()), this, SLOT (previous()));
	connect(nextButton, SIGNAL (released()), this, SLOT (next()));

	createActions();
	createMenus();

	//new QShortcut(QKeySequence(Qt::LeftArrow), this, SLOT(previous()));
	//new QShortcut(QKeySequence(Qt::RightArrow), this, SLOT(next()));

	setWindowTitle(tr("QFaceDetectionAndLabelling (Alpha) by Catree. (2015/09/19)"));
	setMinimumSize(640, 480);
	show();
}

QMyWindow::~QMyWindow(void)
{
}

void QMyWindow::open()
{
	QStringList file_lists = QFileDialog::getOpenFileNames(this, QString(),
        QString(), tr("Images (*.png *.jpg *.jpeg *.bmp);;All files (*.*)"));

	m_vectorOfImageFilepaths.clear();
	for (QStringList::const_iterator it = file_lists.constBegin(); it != file_lists.constEnd(); ++it)
	{
		m_vectorOfImageFilepaths.push_back((*it).toStdString());
		//std::cout << "File=" << (*it).toLocal8Bit().constData() << std::endl;
	}

	if(!m_vectorOfImageFilepaths.empty())
	{
		m_processingDialog = new QProcessingDialog(m_detectEngine, m_vectorOfImageFilepaths, this, Qt::Dialog);
		connect(m_processingDialog->m_threadDetectEngine, SIGNAL(sendProgressFinished()), this, SLOT(updateProgressFinished()));
	}
}

void QMyWindow::save()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		QString(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if(!dir.isEmpty())
	{
		for(std::vector<ImageData>::const_iterator it = m_detectEngine.m_vectorOfImageData.begin(); it != m_detectEngine.m_vectorOfImageData.end(); ++it)
		{
			std::string outputDirectory = dir.toStdString();
			std::string outputFilePath = outputDirectory + "/" + it->m_filename;
			qDebug() << "outputFilePath=" << QString::fromStdString(outputFilePath);
			try
			{
				QFileInfo *fileInfo = new QFileInfo(QString::fromStdString(outputFilePath));
				if(fileInfo->exists())
				{
					QMessageBox::StandardButton reply;
					std::stringstream ss;
					ss << "Do you want to overwrite the following file ?\n" << outputFilePath;
					reply = QMessageBox::question(this, tr("Overwrite ?"), tr(ss.str().c_str()),
						QMessageBox::Yes | QMessageBox::No);
					if (reply == QMessageBox::Yes)
					{
						cv::imwrite(outputFilePath, it->m_img);
					}
				}
				else
				{
					cv::imwrite(outputFilePath, it->m_img);
				}				
			}
			catch(std::exception &e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
		QMessageBox::information(this, tr("Save finished."), tr("All the images have been saved."));
	}
}

void QMyWindow::autoProcess()
{
	//Display dialog to select the directory that contains the images
	QStringList file_lists = QFileDialog::getOpenFileNames(this, QString(),
		QString(), tr("Images (*.png *.jpg *.jpeg *.bmp);;All files (*.*)"));

	m_vectorOfImageFilepaths.clear();
	for (QStringList::const_iterator it = file_lists.constBegin(); it != file_lists.constEnd(); ++it)
	{
		m_vectorOfImageFilepaths.push_back((*it).toStdString());
		//std::cout << "File=" << (*it).toLocal8Bit().constData() << std::endl;
	}

	if(!m_vectorOfImageFilepaths.empty())
	{
		//Display dialog to select the output directory
		QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			QString(),
			QFileDialog::ShowDirsOnly
			| QFileDialog::DontResolveSymlinks);

		if(!dir.isEmpty())
		{
			//Ask to overwrite or not
			QMessageBox::StandardButton reply;
			std::stringstream ss;
			ss << "Do you want to overwrite if the file already exists ?\n";
			reply = QMessageBox::question(this, tr("Overwrite ?"), tr(ss.str().c_str()),
				QMessageBox::Yes | QMessageBox::No);
			bool overwrite = (reply == QMessageBox::Yes);

			//Display the auto process dialog
			m_autoProcessDialog = new QAutoProcessDialog(m_detectEngine, m_vectorOfImageFilepaths, dir.toStdString(), overwrite, this, Qt::Dialog);
			connect(m_autoProcessDialog->m_threadDetectEngine, SIGNAL(sendAutoProgressFinished()), this, SLOT(updateAutoProgressFinished()));
		}
	}
}

void QMyWindow::help()
{
    QMessageBox::about(this, tr("Help Menu"),
            tr("File > Open (or Ctrl+o): load the images.\n"
			"File > Save to directory (or Ctrl+s): save the images into a directory.\n"
			"File > Auto Process: 1) Select the images 2) Select the directory where will e saved the images 3) Select yes to overwrite or no otherise.\n"
			"Double click to remove the label at the specified mouse position.\n"
			"Right click to add a label at the specified mouse position."));
}

void QMyWindow::about()
{
    QMessageBox::about(this, tr("About Menu"),
            tr("This application (QFaceDetectionAndLabelling) has been created by Catree, "
			"under the terms of the GNU General Public License as published by the Free Software Foundation,"
			" either version 3 of the License, or any later version."));
}

void QMyWindow::aboutQt()
{
}

void QMyWindow::previous()
{
	if(!m_detectEngine.m_vectorOfImageData.empty())
	{
		m_currentPosition--;
		if(m_currentPosition < 0)
		{
			m_currentPosition = m_detectEngine.m_vectorOfImageData.size()-1;
		}
		m_imageWidget->putImage(m_detectEngine.m_vectorOfImageData[m_currentPosition].m_img);
		std::stringstream ss;
		ss << m_detectEngine.m_vectorOfImageData[m_currentPosition].m_filename << " - " << (m_currentPosition+1) << "/" << 
			m_detectEngine.m_vectorOfImageData.size();
		m_imageLabel->setText(QString::fromStdString(ss.str()));
	}
}

void QMyWindow::next()
{
	if(!m_detectEngine.m_vectorOfImageData.empty())
	{
		m_currentPosition = (m_currentPosition+1) % m_detectEngine.m_vectorOfImageData.size();
		m_imageWidget->putImage(m_detectEngine.m_vectorOfImageData[m_currentPosition].m_img);
		std::stringstream ss;
		ss << m_detectEngine.m_vectorOfImageData[m_currentPosition].m_filename << " - " << (m_currentPosition+1) << "/" << 
			m_detectEngine.m_vectorOfImageData.size();
		m_imageLabel->setText(QString::fromStdString(ss.str()));
	}
}

void QMyWindow::updateProgressFinished()
{
	m_processingDialog->close();
	if(!m_detectEngine.m_vectorOfImageData.empty())
	{
		m_currentPosition = 0;
		m_imageWidget->putImage(m_detectEngine.m_vectorOfImageData.front().m_img);
		std::stringstream ss;
		ss << m_detectEngine.m_vectorOfImageData[m_currentPosition].m_filename << " - " << (m_currentPosition+1) << "/" << 
			m_detectEngine.m_vectorOfImageData.size();
		m_imageLabel->setText(QString::fromStdString(ss.str()));
	}
}

void QMyWindow::updateAutoProgressFinished()
{
	m_autoProcessDialog->close();
}

void QMyWindow::createActions()
{
    m_openAct = new QAction(tr("&Open"), this);
    m_openAct->setShortcuts(QKeySequence::Open);
    m_openAct->setStatusTip(tr("Open"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_saveAct = new QAction(tr("&Save to directory"), this);
    m_saveAct->setShortcuts(QKeySequence::Save);
    m_saveAct->setStatusTip(tr("Save to directory"));
    connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_autoProcessAct = new QAction(tr("&Auto Process"), this);
    m_autoProcessAct->setStatusTip(tr("Auto Process"));
    connect(m_autoProcessAct, SIGNAL(triggered()), this, SLOT(autoProcess()));

	m_exitAct = new QAction(tr("&Exit"), this);
    m_exitAct->setShortcuts(QKeySequence::Quit);
    m_exitAct->setStatusTip(tr("Exit the application"));
    connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

	m_helpAct = new QAction(tr("&Help"), this);
    m_helpAct->setStatusTip(tr("Show the application's Help box"));
    connect(m_helpAct, SIGNAL(triggered()), this, SLOT(help()));

	m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAct = new QAction(tr("About &Qt"), this);
    m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(m_aboutQtAct, SIGNAL(triggered()), this, SLOT(aboutQt()));
}

void QMyWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_autoProcessAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

	m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_helpAct);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutAct);
    m_helpMenu->addAction(m_aboutQtAct);
}

void QMyWindow::deleteCurrentFace(const int posX, const int posY)
{
	m_detectEngine.eraseFaceAt(m_currentPosition, posX, posY);
	m_imageWidget->putImage(m_detectEngine.m_vectorOfImageData[m_currentPosition].m_img);
}

void QMyWindow::addCurrentFace(const int posX, const int posY, const int labelNumber)
{
	m_detectEngine.addFaceAt(m_currentPosition, posX, posY, labelNumber);
	m_imageWidget->putImage(m_detectEngine.m_vectorOfImageData[m_currentPosition].m_img);
}
