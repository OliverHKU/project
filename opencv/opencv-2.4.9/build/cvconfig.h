/* OpenCV compiled as static or dynamic libs */
#define BUILD_SHARED_LIBS

/* Compile for 'real' NVIDIA GPU architectures */
#define CUDA_ARCH_BIN ""

/* Create PTX or BIN for 1.0 compute capability */
/* #undef CUDA_ARCH_BIN_OR_PTX_10 */

/* NVIDIA GPU features are used */
#define CUDA_ARCH_FEATURES ""

/* Compile for 'virtual' NVIDIA PTX architectures */
#define CUDA_ARCH_PTX ""

/* AVFoundation video libraries */
/* #undef HAVE_AVFOUNDATION */

/* V4L capturing support */
/* #undef HAVE_CAMV4L */

/* V4L2 capturing support */
#define HAVE_CAMV4L2

/* Carbon windowing environment */
/* #undef HAVE_CARBON */

/* AMD's Basic Linear Algebra Subprograms Library*/
/* #undef HAVE_CLAMDBLAS */

/* AMD's OpenCL Fast Fourier Transform Library*/
/* #undef HAVE_CLAMDFFT */

/* Cocoa API */
/* #undef HAVE_COCOA */

/* C= */
/* #undef HAVE_CSTRIPES */

/* NVidia Cuda Basic Linear Algebra Subprograms (BLAS) API*/
/* #undef HAVE_CUBLAS */

/* NVidia Cuda Runtime API*/
/* #undef HAVE_CUDA */

/* NVidia Cuda Fast Fourier Transform (FFT) API*/
/* #undef HAVE_CUFFT */

/* IEEE1394 capturing support */
/* #undef HAVE_DC1394 */

/* IEEE1394 capturing support - libdc1394 v2.x */
#define HAVE_DC1394_2

/* DirectShow Video Capture library */
/* #undef HAVE_DSHOW */

/* Eigen Matrix & Linear Algebra Library */
/* #undef HAVE_EIGEN */

/* FFMpeg video library */
#define HAVE_FFMPEG

/* ffmpeg's libswscale */
#define HAVE_FFMPEG_SWSCALE

/* ffmpeg in Gentoo */
#define HAVE_GENTOO_FFMPEG

/* GStreamer multimedia framework */
/* #undef HAVE_GSTREAMER */

/* GTK+ 2.0 Thread support */
/* #undef HAVE_GTHREAD */

/* Windows Runtime support */
/* #undef HAVE_WINRT */

/* Win32 UI */
/* #undef HAVE_WIN32UI */

/* GTK+ 2.x toolkit */
/* #undef HAVE_GTK */

/* Apple ImageIO Framework */
/* #undef HAVE_IMAGEIO */

/* Intel Perceptual Computing SDK library */
/* #undef HAVE_INTELPERC */

/* Intel Integrated Performance Primitives */
/* #undef HAVE_IPP */

/* JPEG-2000 codec */
#define HAVE_JASPER

/* IJG JPEG codec */
#define HAVE_JPEG

/* libpng/png.h needs to be included */
#define HAVE_LIBPNG_PNG_H

/* V4L/V4L2 capturing support via libv4l */
/* #undef HAVE_LIBV4L */

/* Microsoft Media Foundation Capture library */
/* #undef HAVE_MSMF */

/* NVidia Video Decoding API*/
/* #undef HAVE_NVCUVID */

/* OpenCL Support */
#define HAVE_OPENCL
/* #undef HAVE_OPENCL_STATIC */

/* OpenEXR codec */
#define HAVE_OPENEXR

/* OpenGL support*/
#define HAVE_OPENGL

/* OpenNI library */
/* #undef HAVE_OPENNI */

/* PNG codec */
#define HAVE_PNG

/* Qt support */
#define HAVE_QT

/* Qt OpenGL support */
#define HAVE_QT_OPENGL

/* QuickTime video libraries */
/* #undef HAVE_QUICKTIME */

/* QTKit video libraries */
/* #undef HAVE_QTKIT */

/* Intel Threading Building Blocks */
/* #undef HAVE_TBB */

/* TIFF codec */
#define HAVE_TIFF

/* Unicap video capture library */
/* #undef HAVE_UNICAP */

/* Video for Windows support */
/* #undef HAVE_VFW */

/* V4L2 capturing support in videoio.h */
/* #undef HAVE_VIDEOIO */

/* Win32 UI */
/* #undef HAVE_WIN32UI */

/* XIMEA camera support */
/* #undef HAVE_XIMEA */

/* Xine video library */
/* #undef HAVE_XINE */

/* Define if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */
