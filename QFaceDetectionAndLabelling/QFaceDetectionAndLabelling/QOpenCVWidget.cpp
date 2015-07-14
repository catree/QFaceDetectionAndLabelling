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
#include <QtWidgets/QInputDialog>

#include "QOpenCVWidget.h"

/**
	@author: Catree
	@date: 2015/07/14
**/


// Constructor
QOpenCVWidget::QOpenCVWidget(QWidget* parent) : QWidget(parent)
{
	parentInstance = (QMyWindow*) parent;
    m_pLayout		= new QVBoxLayout;
    m_pImageLabel	= new QLabel;
	QImage dummy(parent->size().width(), parent->size().height(), QImage::Format_RGB32);
    m_image			= dummy;
    m_pLayout->addWidget(m_pImageLabel);
	for (int x = 0; x < dummy.width(); x++)
	{
		for (int y =0; y < dummy.height(); y++)
		{
            m_image.setPixel(x, y, qRgb(x, y, y));
        }
    }
    m_pImageLabel->setPixmap(QPixmap::fromImage(m_image));
	m_pImageLabel->setScaledContents(true);
    setLayout(m_pLayout);
}

QOpenCVWidget::~QOpenCVWidget(void)
{
}

void QOpenCVWidget::putImage(const cv::Mat& cvimage)
{
	if(!cvimage.empty())
	{
		cvimage.copyTo(m_matImage);
		m_image = MatToQImage(cvimage);
		m_pImageLabel->setPixmap(QPixmap::fromImage(m_image));
		repaint();
	}
}

void QOpenCVWidget::mouseReleaseEvent(QMouseEvent *event)
{
	//qDebug() << m_pImageLabel->mapFromParent(event->pos());
	Qt::MouseButton buttonPressed = event->button();
	QPoint position = event->pos();
	if(buttonPressed == Qt::RightButton)
	{
		bool ok;
		QString text = QInputDialog::getText(this, tr("Label number ?"),
			tr("Label number:"), QLineEdit::Normal,
			QString(), &ok);
		if (ok && !text.isEmpty())
		{
			if(!m_matImage.empty())
			{
				//@see: https://forum.qt.io/topic/5632/solved-cursor-and-image-coordinates-problem-with-scaling/8
				int localX = m_pImageLabel->mapFromParent(position).x();
				int localY = m_pImageLabel->mapFromParent(position).y();

				//qDebug() << "widget=" << this->width() << "x" << this->height();
				qDebug() << "m_pImageLabel=" << m_pImageLabel->width() << "x" << m_pImageLabel->height();
				qDebug() << "m_matImage=" << m_matImage.cols << "x" << m_matImage.rows;
				int imgX = localX * m_matImage.cols / (double) m_pImageLabel->width();
				int imgY = localY * m_matImage.rows / (double) m_pImageLabel->height();
				qDebug() << "localX=" << localX << " ; imgX=" << imgX;
				qDebug() << "localY=" << localY << " ; imgY=" << imgY;

				try
				{
					parentInstance->addCurrentFace(imgX, imgY, text.toInt());
				}
				catch(std::exception &e)
				{
					std::cerr << e.what() << std::endl;
				}
			}
		}
	}
}

void QOpenCVWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	//qDebug() << m_pImageLabel->mapFromParent(event->pos());
	Qt::MouseButton buttonPressed = event->button();
	QPoint position = event->pos();
	if(buttonPressed == Qt::LeftButton)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, tr("Remove this label ?"), tr("Do you want to remove this label ?"),
			QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			if(!m_matImage.empty())
			{
				//@see: https://forum.qt.io/topic/5632/solved-cursor-and-image-coordinates-problem-with-scaling/8
				int localX = m_pImageLabel->mapFromParent(position).x();
				int localY = m_pImageLabel->mapFromParent(position).y();

				//qDebug() << "widget=" << this->width() << "x" << this->height();
				qDebug() << "m_pImageLabel=" << m_pImageLabel->width() << "x" << m_pImageLabel->height();
				qDebug() << "m_matImage=" << m_matImage.cols << "x" << m_matImage.rows;
				int imgX = localX * m_matImage.cols / (double) m_pImageLabel->width();
				int imgY = localY * m_matImage.rows / (double) m_pImageLabel->height();
				qDebug() << "localX=" << localX << " ; imgX=" << imgX;
				qDebug() << "localY=" << localY << " ; imgY=" << imgY;

				parentInstance->deleteCurrentFace(imgX, imgY);
			}
		}
	}
}
