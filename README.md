# Digital Photo Album
by: Asher Vogel

Have you ever wanted an easy way to build a digital photo album to preserve your treasured memories? This C program makes it super easy to do just that! Simply pass in the path to your photos from the command line, follow the prompts to rotate and add captions to your photos, and this program will take care of the rest. It will create an oriented thumbnail and medium-sized version of all your photos in the current directory, as well as an HTML photo album full of captioned thumbnails that linked to bigger images. Just like that, you have a photo album you can cherish forever!

## Usage

```
git clone git@github.com:ashervogel17/DigitalPhotoAlbum.git
cd DigitalPhotoAlbum
make
```
Then navigate to a directory where you want to the photo album to go and run the following:
```
/path/to/DigitalPhotoAlbum/album /path/to/photo/directory/*
```
The command line prompt supports wild card expressions, single files, and even multiple arguments, so you can pull in photos from wherever they are stored on your computer.

## Solution Architecture

The key to this program's success is taking advantage of concurrency. While the program is waiting for the user to input a caption or rotate a photo, it is processing one or more images in the background. This means minimal computing downtime and as seamless an experience as possible for the user. Here is a diagram of how the processes' lifelines: [Process Lifelifes](./process_lifelines.pdf).