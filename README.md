Face Detection + Auto Labelling (Alpha version)
====================

Face Detection on school year group photo and automatic labelling.

![Demo](https://github.com/catree/QFaceDetectionAndLabelling/blob/master/Demo.gif)

« <a href="https://commons.wikimedia.org/wiki/File:Condorcet_1897.jpg#/media/File:Condorcet_1897.jpg">Condorcet 1897</a> » by Pierre Petit — 
<span class="int-own-work" lang="fr">Travail personnel</span>. Licensed under <a title="Creative Commons Attribution-Share Alike 3.0" 
href="http://creativecommons.org/licenses/by-sa/3.0">CC BY-SA 3.0</a> via <a href="https://commons.wikimedia.org/wiki/">Wikimedia Commons</a>.


## Usage
- File > Open: Open a dialog to select the desired images
- When an image is loaded, a double left click will delete (after a confirmation) the label located under the mouse cursor (known issue: it is sometimes buggy, retry multiple times).
- When an image is loaded, a right click will add a label. A dialog will show and ask for the label number.
- Use the "<" and ">" buttons to load the previous and next images.
- File > Save to directory: Open a dialog to select the directory where will be saved the images. If a file with the same name already exists, it will be asked if we want to overwrite or not the file.


## Development environment
- Windows 7 64 bits
- Visual Studio 2010
- OpenCV 2.4.10
- Qt 5.5.0
- binarie(Alpha version) compiled in 32 bits release mode


## Known issues
- The deletion of a label by a left double click seems to be sometimes buggy and require to try many times 


## TODO
- [ ] Add a setting dialog to allow the modification of some parameters (Cascade classifier learning file, size of the label box, etc.)
- [ ] Try to find a way to use more classical fonts than those provided by OpenCV
- [ ] Try to find a way to display a better image than the raw RGB888
