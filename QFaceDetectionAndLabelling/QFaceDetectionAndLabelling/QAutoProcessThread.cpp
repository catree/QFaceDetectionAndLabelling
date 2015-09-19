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


#include "QAutoProcessThread.h"

/**
	@author: Catree
	@date: 2015/09/19
**/


QAutoProcessThread::QAutoProcessThread(DetectAndLabelizeFaces *detectEngine, const std::vector<std::string> &vectorOfImageFilepaths, const std::string &outputDir, const bool overwrite, QObject *parent) 
	: QThread(parent), m_detectEngine(detectEngine), m_vectorOfImageFilepaths(vectorOfImageFilepaths), m_stop(false), m_outputDirectory(outputDir), m_overwrite(overwrite)
{
	qRegisterMetaType<std::string>("std::string");
}

QAutoProcessThread::~QAutoProcessThread(void)
{
}

void QAutoProcessThread::run()
{
	//Clear the data
	m_detectEngine->m_vectorOfImageData.clear();

	if(m_detectEngine->m_faceCascade.empty())
	{
		emit sendError();
	}
	else
	{
		size_t nb = 0;
		for(std::vector<std::string>::const_iterator it = m_vectorOfImageFilepaths.begin(); it != m_vectorOfImageFilepaths.end(); ++it, nb++)
		{
			if(m_stop)
			{
				//Clear the data
				m_detectEngine->m_vectorOfImageData.clear();

				break;
			}

			//Get output filename
			QFileInfo *fileInfoImg = new QFileInfo(QString::fromStdString(*it));
			std::string outputFilePath = m_outputDirectory + "/" + fileInfoImg->fileName().toStdString();
			//qDebug() << "QAutoProcessDialog::updateSave outputFilePath=" << QString::fromStdString(outputFilePath);

			QFileInfo *fileInfo = new QFileInfo(QString::fromStdString(outputFilePath));
			std::stringstream ss;
			if(m_overwrite || !fileInfo->exists())
			{
				//Process
				m_detectEngine->process(*it);
				ss.str("");
				
				size_t nbDetectedFaces = m_detectEngine->m_vectorOfImageData.back().m_vectorOfDetectedFaces.size();
				ss << "Found: " << nbDetectedFaces << " faces for " << fileInfoImg->fileName().toStdString() << ".";
				emit sendTextEdit(ss.str());

				//Save
				try
				{
					cv::imwrite(outputFilePath, m_detectEngine->m_vectorOfImageData.back().m_img);
					ss.str("");
					ss << "Successfully saved image to: " << outputFilePath << " !\n";
					emit sendTextEdit(ss.str());
				}
				catch(std::exception &e)
				{
					std::cerr << e.what() << std::endl;
				}
			}

			ss.str("");
			ss << (nb+1) << "/" << m_vectorOfImageFilepaths.size();
			emit sendProgressLabel(ss.str());

			int pourcentage = (int) ((nb+1) * 100 / m_vectorOfImageFilepaths.size());
			emit sendProgressBar(pourcentage);

			//Clear the data
			m_detectEngine->m_vectorOfImageData.clear();
		}
	}

	emit sendAutoProgressFinished();
}
