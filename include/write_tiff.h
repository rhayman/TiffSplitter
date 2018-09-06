#ifndef WRITE_TIFF_H_
#define WRITE_TIFF_H_

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/cvdef.h>
#include <opencv2/core/utility.hpp>
#include <vector>
#include <utility>
#include <exception>
#include <string>
#include <iostream>
#include <iomanip>
#include <tiffio.h>
#include <opencv2/core.hpp>
#include <exception>
#include "bitstrm.hpp"
#include "utils.hpp"

namespace cv
{
// native simple TIFF codec
enum TiffCompression
{
    TIFF_UNCOMP = 1,
    TIFF_HUFFMAN = 2,
    TIFF_PACKBITS = 32773
};

enum TiffByteOrder
{
    TIFF_ORDER_II = 0x4949,
    TIFF_ORDER_MM = 0x4d4d
};


enum  TiffTag
{
    TIFF_TAG_WIDTH  = 256,
    TIFF_TAG_HEIGHT = 257,
    TIFF_TAG_BITS_PER_SAMPLE = 258,
    TIFF_TAG_COMPRESSION = 259,
    TIFF_TAG_PHOTOMETRIC = 262,
    TIFF_TAG_STRIP_OFFSETS = 273,
    TIFF_TAG_STRIP_COUNTS = 279,
    TIFF_TAG_SAMPLES_PER_PIXEL = 277,
    TIFF_TAG_ROWS_PER_STRIP = 278,
    TIFF_TAG_PLANAR_CONFIG = 284,
    TIFF_TAG_COLOR_MAP = 320
};


enum TiffFieldType
{
    TIFF_TYPE_BYTE = 1,
    TIFF_TYPE_SHORT = 3,
    TIFF_TYPE_LONG = 4
};

class BaseImageEncoder
{
public:
	BaseImageEncoder();
    virtual ~BaseImageEncoder() {}
    virtual bool isFormatSupported( int depth ) const;

    virtual bool setDestination( const cv::String& filename );
    virtual bool setDestination( std::vector<uchar>& buf );
    virtual bool write( const cv::Mat& img, const std::vector<int>& params ) = 0;

    virtual cv::String getDescription() const;
    // virtual ImageEncoder newEncoder() const;

    // virtual void throwOnEror() const;

protected:
    cv::String m_description;

    cv::String m_filename;
    std::vector<uchar>* m_buf;
    bool m_buf_supported;

    cv::String m_last_error;
};



class TiffWriter : public BaseImageEncoder
{
public:
	TiffWriter();
	virtual ~TiffWriter();
	bool isFormatSupported( int depth ) const;
	bool  write( const cv::Mat& img, const std::vector<int>& params );

	virtual bool isOpened();
	virtual bool open(cv::String outputPath);
    virtual bool close();
	virtual TiffWriter& operator << (cv::Mat& frame);
    bool writeSIHdr(const std::string swTag, const std::string imDescTag);

protected:
    void  writeTag( cv::WLByteStream& strm, TiffTag tag,
                    TiffFieldType fieldType,
                    int count, int value );
    bool writeLibTiff( const cv::Mat& img, const std::vector<int>& params );
    bool writeHdr( const cv::Mat& img );
    std::string type2str(int type);
	TIFF* m_tif;
	TIFF* pTiffHandle;
	int frame_number = 1;
	double time_stamp = 0;
	bool opened = false;
};

} // namespace cv

#endif