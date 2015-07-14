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


#include "QThreadFaceDetectionAndLabelling.h"

/**
	@author: Catree
	@date: 2015/07/14
**/


QThreadFaceDetectionAndLabelling::QThreadFaceDetectionAndLabelling(DetectAndLabelizeFaces *detectEngine, const std::vector<std::string> &vectorOfImageFilepaths, QObject *parent) 
	: QThread(parent), m_detectEngine(detectEngine), m_vectorOfImageFilepaths(vectorOfImageFilepaths), m_stop(false)
{
	qRegisterMetaType<std::string>("std::string");
}

QThreadFaceDetectionAndLabelling::~QThreadFaceDetectionAndLabelling(void)
{
}

void QThreadFaceDetectionAndLabelling::run()
{
	//Clear the data
	m_detectEngine->m_vectorOfImageData.clear();
	size_t nb = 0;
	for(std::vector<std::string>::const_iterator it = m_vectorOfImageFilepaths.begin(); it != m_vectorOfImageFilepaths.end(); ++it, nb++)
	{
		if(m_stop)
		{
			break;
		}

		std::stringstream ss;
		ss << "Read: " << *it;
		emit sendTextEdit(ss.str());
		m_detectEngine->process(*it);
		ss.str("");
		ss << "Found: " << m_detectEngine->m_vectorOfImageData[nb].m_vectorOfDetectedFaces.size() << " faces.\n";
		emit sendTextEdit(ss.str());

		ss.str("");
		ss << (nb+1) << "/" << m_vectorOfImageFilepaths.size();
		emit sendProgressLabel(ss.str());

		int pourcentage = (int) ((nb+1) * 100 / m_vectorOfImageFilepaths.size());
		emit sendProgressBar(pourcentage);
	}

	emit sendProgressFinished();
}
