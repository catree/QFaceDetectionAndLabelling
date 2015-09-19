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


#ifndef QDETECTFACESANDLABELIZE_H
#define QDETECTFACESANDLABELIZE_H

#include <iostream>
#include <algorithm>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

#define DEBUG 0

/**
	@author: Catree
	@date: 2015/07/14
	@dateUpdate: 2015/09/19
**/


typedef struct face_info_t {
  cv::Point face_center;
  cv::Size face_size;
  cv::Point label_center;
  cv::Size label_box_size;
  cv::Size img_size;
  int label;

  face_info_t() : face_center(), face_size(), label_center(), label_box_size(), img_size(), label()
  {
  }

  face_info_t(const cv::Point &center, const cv::Size &size, const cv::Point &lbl_center, const cv::Size _img_size) 
	  : face_center(center), face_size(size), label_center(lbl_center), label_box_size(), img_size(_img_size), label()
  {
  }

  bool operator<(const face_info_t& face_info) const
  {
	  int top1 = (int) (face_center.y - face_size.height*0.5);
	  int bottom1 = (int) (face_center.y + face_size.height*0.5);
	  int top2 = (int) (face_info.face_center.y - face_info.face_size.height*0.5);
	  int bottom2 = (int) (face_info.face_center.y + face_info.face_size.height*0.5);
	  if( (top1 >= top2 && top1 <= bottom2) || (bottom1 >= top2 && bottom1 <= bottom2) || 
		  (top2 >= top1 && top2 <= bottom1) || (bottom2 >= top1 && bottom2 <= bottom1) )
	  {
		  return (face_center.x < face_info.face_center.x);
	  }

	  return (face_center.y < face_info.face_center.y);
  }
} face_info_t;


class ImageData
{
public:
	cv::Mat m_img;
	cv::Mat m_originalImg;
	std::vector<face_info_t> m_vectorOfDetectedFaces;
	std::string m_filename;
	std::string m_filepath;
	int m_medianFaceWidth;

	ImageData() : m_img(), m_originalImg(), m_vectorOfDetectedFaces(), m_filename(), m_filepath(), m_medianFaceWidth()
	{
	}

	ImageData(const cv::Mat &_img, const cv::Mat _originalImg, const std::vector<face_info_t> _vectorOfDetectedFaces, const std::string _filepath, const int _medianFaceWidth) : 
		m_img(_img), m_originalImg(_originalImg), m_vectorOfDetectedFaces(_vectorOfDetectedFaces), m_filename(), m_filepath(_filepath), m_medianFaceWidth(_medianFaceWidth)
	{
		QFileInfo *fileInfo = new QFileInfo(QString::fromStdString(_filepath));
		m_filename = fileInfo->fileName().toStdString();
	}
};


class DetectAndLabelizeFaces
{
public:
	std::vector<ImageData> m_vectorOfImageData;
	cv::CascadeClassifier m_faceCascade;

	DetectAndLabelizeFaces() : m_vectorOfImageData(), m_faceCascade("haarcascade_frontalface_alt2.xml")
	{
	}

	void process(const std::string &imageFilepath)
	{
		if(!m_faceCascade.empty())
		{
			cv::Mat originalImg = readImage(imageFilepath);
			if(!originalImg.empty())
			{
				cv::Mat img;
				originalImg.copyTo(img);
				int medianFaceWidth = 40;
				std::vector<face_info_t> vectorOfFaces = detectFacesAndDisplayLabels(img, m_faceCascade, medianFaceWidth);
				m_vectorOfImageData.push_back(ImageData(img, originalImg, vectorOfFaces, imageFilepath, medianFaceWidth));
			}
		}
	}

	void process(const std::vector<std::string> &vectorOfImageFilepaths)
	{
		m_vectorOfImageData.clear();
		for(std::vector<std::string>::const_iterator it = vectorOfImageFilepaths.begin(); it != vectorOfImageFilepaths.end(); ++it)
		{
			process(*it);
		}
	}

	void eraseFaceAt(const size_t index, const int posX, const int posY)
	{
		if(!m_vectorOfImageData.empty() && index < m_vectorOfImageData.size())
		{
			for(std::vector<face_info_t>::iterator it = m_vectorOfImageData[index].m_vectorOfDetectedFaces.begin(); 
				it != m_vectorOfImageData[index].m_vectorOfDetectedFaces.end();)
			{
				cv::Rect r_label(it->label_center.x-it->label_box_size.width*0.5, it->label_center.y-it->label_box_size.height*0.5, 
					it->label_box_size.width, it->label_box_size.height);
				//std::cout << "r_label=" << it->label << " ; " << r_label << std::endl;
				if(r_label.contains(cv::Point(posX, posY)))
				{
					//delete face
					qDebug() << "Delete";
					it = m_vectorOfImageData[index].m_vectorOfDetectedFaces.erase(it);
				}
				else
				{
					++it;
				}
			}

			m_vectorOfImageData[index].m_originalImg.copyTo(m_vectorOfImageData[index].m_img);

			std::sort(m_vectorOfImageData[index].m_vectorOfDetectedFaces.begin(), m_vectorOfImageData[index].m_vectorOfDetectedFaces.end());

			////Default size
			//int size = std::min(m_vectorOfImageData[index].m_originalImg.rows, m_vectorOfImageData[index].m_originalImg.cols) / 24;
			//cv::Size label_box_size(size, size);
			cv::Size label_box_size(m_vectorOfImageData[index].m_medianFaceWidth*0.5, m_vectorOfImageData[index].m_medianFaceWidth*0.5);
			int label = 1;
			for(std::vector<face_info_t>::iterator it = m_vectorOfImageData[index].m_vectorOfDetectedFaces.begin(); 
				it != m_vectorOfImageData[index].m_vectorOfDetectedFaces.end(); ++it, label++)
			{
				it->label = label;
				displayNumbers(m_vectorOfImageData[index].m_img, label, it->label_center, label_box_size);
			}			
		}
	}	

	void addFaceAt(const size_t index, const int posX, const int posY, const int labelNumber)
	{
		if(!m_vectorOfImageData.empty() && index < m_vectorOfImageData.size())
		{
			////Default size
			//int size = std::min(m_vectorOfImageData[index].m_originalImg.rows, m_vectorOfImageData[index].m_originalImg.cols) / 24;
			//cv::Size label_box_size(size, size);
			cv::Size label_box_size(m_vectorOfImageData[index].m_medianFaceWidth*0.5, m_vectorOfImageData[index].m_medianFaceWidth*0.5);
			face_info_t face(cv::Point(posX, posY), cv::Size(40,40), cv::Point(posX, posY), m_vectorOfImageData[index].m_originalImg.size());
			face.label_box_size = label_box_size;

			face.label = labelNumber;
			m_vectorOfImageData[index].m_vectorOfDetectedFaces.push_back(face);
			displayNumbers(m_vectorOfImageData[index].m_img, labelNumber, face.label_center, label_box_size);
		}
	}

	static std::vector<face_info_t> removeSameFaces(const std::vector<face_info_t> &vectorOfFaces)
	{
		std::vector<face_info_t> vectorOfUniqueFaces;
		std::vector<size_t> vectorOfDuplicateIndexes;
		for(size_t i = 0; i < vectorOfFaces.size(); i++)
		{
			int top = (int) (vectorOfFaces[i].face_center.x - vectorOfFaces[i].face_size.width*0.5);
			int left = (int) (vectorOfFaces[i].face_center.y - vectorOfFaces[i].face_size.height*0.5);
			cv::Rect r1(top, left, vectorOfFaces[i].face_size.width, vectorOfFaces[i].face_size.height);

			bool duplicate = false;
			for(size_t j = i+1; j < vectorOfFaces.size(); j++)
			{
				if(std::find(vectorOfDuplicateIndexes.begin(), vectorOfDuplicateIndexes.end(), j) == vectorOfDuplicateIndexes.end())
				{
					top = (int) (vectorOfFaces[j].face_center.x - vectorOfFaces[j].face_size.width*0.5);
					left = (int) (vectorOfFaces[j].face_center.y - vectorOfFaces[j].face_size.height*0.5);
					cv::Rect r2(top, left, vectorOfFaces[i].face_size.width, vectorOfFaces[i].face_size.height);
					cv::Rect r_intersection = r1 & r2;

					if(r_intersection.width > 0 && r_intersection.height > 0)
					{
						duplicate = true;
						vectorOfDuplicateIndexes.push_back(j);
					}
				}
			}

			if(!duplicate)
			{
				vectorOfUniqueFaces.push_back(vectorOfFaces[i]);
			}
		}

		return vectorOfUniqueFaces;
	}

	static void displayNumbers(cv::Mat &img, const int number, const cv::Point &center, const cv::Size &label_box_size=cv::Size(30,30), 
		const cv::Scalar label_box_bcg_color=cv::Scalar(255,255,255))
	{
		if(img.empty())
		{
			return;
		}

		cv::Mat label_box(label_box_size, CV_8UC3);
		label_box.setTo(label_box_bcg_color);

		std::stringstream ss;
		ss << number;
		double factor = label_box_size.width / 30.0;
		try
		{
			if(number < 10)
			{
				cv::putText(label_box, ss.str(), cv::Point(label_box.cols/2-6*factor, label_box.rows/2+5*factor), cv::FONT_HERSHEY_PLAIN, factor, cv::Scalar(0,0,0), 2);
			}
			else
			{
				cv::putText(label_box, ss.str(), cv::Point(label_box.cols/2-10*factor, label_box.rows/2+5*factor), cv::FONT_HERSHEY_PLAIN, factor, cv::Scalar(0,0,0), 2);
			}	

			label_box.copyTo(img(cv::Rect(center.x, center.y, label_box.cols, label_box.rows)));
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	static std::vector<face_info_t> detectFacesAndDisplayLabels(cv::Mat &img, cv::CascadeClassifier &face_cascade, int &medianFaceWidth)
	{
		std::vector<cv::Rect> faces;
		face_cascade.detectMultiScale(img, faces);

		//Default size
		int size = std::min(img.rows, img.cols) / 24;
		cv::Size label_box_size(size, size);
		std::vector<face_info_t> vectorOfFaces(faces.size());
		std::vector<int> vectorOfFaceWidths(faces.size());
		for (size_t i = 0; i < faces.size(); i++)
		{
			cv::Point center((int) (faces[i].x + faces[i].width*0.5), (int) (faces[i].y + faces[i].height*0.5));

	#if DEBUG
			ellipse(img, center, cv::Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar(255, 0, 255), 2);
	#endif

			vectorOfFaceWidths[i] = faces[i].size().width;
			cv::Point label_box(center.x - size/2, center.y + faces[i].height - size/2);
			vectorOfFaces[i] = face_info_t(center, faces[i].size(), label_box, img.size());
		}

		medianFaceWidth = (int) median(vectorOfFaceWidths);
		label_box_size.width = medianFaceWidth*0.5;
		label_box_size.height = medianFaceWidth*0.5;
		for(std::vector<face_info_t>::iterator it = vectorOfFaces.begin(); it != vectorOfFaces.end(); ++it)
		{
			cv::Point label_box(it->face_center.x - label_box_size.width/2, it->face_center.y + it->face_size.height - label_box_size.height/2);
			it->label_box_size = label_box_size;
			it->label_center = label_box;
		}

		std::sort (vectorOfFaces.begin(), vectorOfFaces.end());
		vectorOfFaces = removeSameFaces(vectorOfFaces);

		int label = 1;
		for(std::vector<face_info_t>::iterator it = vectorOfFaces.begin(); it != vectorOfFaces.end(); ++it, label++)
		{
			it->label = label;
			//it->label_box_size = label_box_size;
			displayNumbers(img, label, it->label_center, it->label_box_size);
		}

		return vectorOfFaces;
	}

	static cv::Mat readImage(const std::string &filepath)
	{
		cv::Mat img;
		try
		{
			img = cv::imread(filepath);
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
		return img;
	}

	//@see: http://stackoverflow.com/a/24235744
	static double median(std::vector<int> &v)
	{
		if(v.size() > 0)
		{
			size_t n = v.size() / 2;
			std::nth_element(v.begin(), v.begin()+n, v.end());
			int vn = v[n];
			if(v.size() % 2 == 1)
			{
				return vn;
			}
			else
			{
				std::nth_element(v.begin(), v.begin()+n-1, v.end());
				return 0.5*(vn+v[n-1]);
			}
		}

		throw std::exception("Vector is empty !");
	}
};


#endif
