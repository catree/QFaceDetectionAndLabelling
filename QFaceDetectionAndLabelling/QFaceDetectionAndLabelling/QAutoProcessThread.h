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


#ifndef QAUTOPROCESSTHREAD_H
#define QAUTOPROCESSTHREAD_H

#include <QThread>
#include "DetectAndLabelizeFaces.h"

/**
	@author: Catree
	@date: 2015/09/19
**/


class QAutoProcessThread : public QThread
{
	Q_OBJECT
signals:
	void sendTextEdit(const std::string &text);
	void sendProgressBar(const int value);
	void sendProgressLabel(const std::string &label);
	void sendAutoProgressFinished();
	void sendError();

public:
	DetectAndLabelizeFaces *m_detectEngine;
	bool m_stop;
	std::string m_outputDirectory;
	bool m_overwrite;

	QAutoProcessThread(DetectAndLabelizeFaces *detectEngine, const std::vector<std::string> &vectorOfImageFilepaths, const std::string &outputDir, const bool overwrite, QObject *parent=0);
	~QAutoProcessThread(void);


private:
	std::vector<std::string> m_vectorOfImageFilepaths;
	void run();
};

#endif
