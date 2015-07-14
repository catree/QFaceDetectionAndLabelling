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


#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv2/opencv.hpp>
#include <QtGui/QPixmap>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QImage>

#include "MatToQImage.h"
#include "QMyWindow.h"

class QMyWindow;

/**
	@author: Catree
	@date: 2015/07/14
**/


class QOpenCVWidget : public QWidget
{
    private:
        QLabel *m_pImageLabel;
        QVBoxLayout *m_pLayout;
        
        QImage m_image;
		cv::Mat m_matImage;
		QMyWindow *parentInstance;
        
    public:
        QOpenCVWidget(QWidget *parent = 0);
        ~QOpenCVWidget(void);
        void putImage(const cv::Mat&);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif
