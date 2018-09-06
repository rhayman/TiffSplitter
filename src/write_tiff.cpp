#include "../include/write_tiff.h"

namespace cv
{

static const char fmtSignTiffII[] = "II\x2a\x00";
static const char fmtSignTiffMM[] = "MM\x00\x2a";

BaseImageEncoder::BaseImageEncoder()
{
	m_buf_supported = false;
}

bool BaseImageEncoder::isFormatSupported( int depth ) const
{
    return depth == CV_8U;
}

cv::String BaseImageEncoder::getDescription() const
{
    return m_description;
}

bool BaseImageEncoder::setDestination( const cv::String& filename )
{
    m_filename = filename;
    m_buf = 0;
    return true;
}

bool BaseImageEncoder::setDestination( std::vector<uchar>& buf )
{
    if( !m_buf_supported )
        return false;
    m_buf = &buf;
    m_buf->clear();
    m_filename = cv::String();
    return true;
}

TiffWriter::TiffWriter()
{
	m_description = "TIFF Files (*.tiff;*.tif)";
	m_buf_supported = true;
}


TiffWriter::~TiffWriter()
{
	if (opened)
		TIFFClose(m_tif);
    if ( pTiffHandle )
        TIFFClose(pTiffHandle);
}

bool TiffWriter::isFormatSupported( int depth ) const
{
    return depth == CV_8U || depth == CV_16U || depth == CV_32F;
}

static void readParam(const std::vector<int>& params, int key, int& value)
{
    for(size_t i = 0; i + 1 < params.size(); i += 2)
        if(params[i] == key)
        {
            value = params[i+1];
            break;
        }
}

bool  TiffWriter::writeLibTiff( const cv::Mat& img, const std::vector<int>& params)
{
    int channels = img.channels();
    int width = img.cols, height = img.rows;
    int depth = img.depth();

    int bitsPerChannel = -1;
    switch (depth)
    {
        case CV_8U:
        {
            bitsPerChannel = 8;
            break;
        }
        case CV_16U:
        {
            bitsPerChannel = 16;
            break;
        }
        case CV_16S:
        {
            bitsPerChannel = 16;
            break;
        }
        default:
        {
            return false;
        }
    }

    const int bitsPerByte = 16;
    size_t fileStep = (width * channels * bitsPerChannel) / bitsPerByte;// = image_width (with 1 channel)

    int rowsPerStrip = (int)((1 << 13)/fileStep);
    readParam(params, TIFFTAG_ROWSPERSTRIP, rowsPerStrip);

    rowsPerStrip = height;

    // do NOT put "wb" as the mode, because the b means "big endian" mode, not "binary" mode.
    // http://www.remotesensing.org/libtiff/man/TIFFOpen.3tiff.html
    // IMPORTANT: Note the "w8" option here - this is what allows writing to the bigTIFF format
    // possible ('normal' tiff would be just "w")
    if (!(isOpened()))
    	pTiffHandle = TIFFOpen(m_filename.c_str(), "w8");
    else
    	pTiffHandle = m_tif;
    if (!pTiffHandle)
    {
        return false;
    }

    // defaults for now, maybe base them on params in the future
    int    compression  = COMPRESSION_NONE;
    int    predictor    = PREDICTOR_HORIZONTAL;
    int    units        = RESUNIT_INCH;
    double xres         = 72.0;
    double yres         = 72.0;
    int    sampleformat = SAMPLEFORMAT_INT;
    int    orientation  = ORIENTATION_TOPLEFT;
    int planarConfig = 1;

    readParam(params, TIFFTAG_COMPRESSION, compression);
    readParam(params, TIFFTAG_PREDICTOR, predictor);

    // std::string desc = "Frame Number = " + std::to_string(frame_number) + "\nFrame Timestamp(s) = " + std::to_string(time_stamp);

    int   colorspace = channels > 1 ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;

    if ( !TIFFSetField(pTiffHandle, TIFFTAG_IMAGEWIDTH, width)
      || !TIFFSetField(pTiffHandle, TIFFTAG_IMAGELENGTH, height)
      || !TIFFSetField(pTiffHandle, TIFFTAG_BITSPERSAMPLE, bitsPerChannel)
      || !TIFFSetField(pTiffHandle, TIFFTAG_COMPRESSION, compression)
      || !TIFFSetField(pTiffHandle, TIFFTAG_PHOTOMETRIC, colorspace)
      || !TIFFSetField(pTiffHandle, TIFFTAG_SAMPLESPERPIXEL, channels)
      || !TIFFSetField(pTiffHandle, TIFFTAG_PLANARCONFIG, planarConfig)
      || !TIFFSetField(pTiffHandle, TIFFTAG_ROWSPERSTRIP, rowsPerStrip)
      || !TIFFSetField(pTiffHandle, TIFFTAG_RESOLUTIONUNIT, units)
      || !TIFFSetField(pTiffHandle, TIFFTAG_XRESOLUTION, xres)
      || !TIFFSetField(pTiffHandle, TIFFTAG_YRESOLUTION, yres)
      || !TIFFSetField(pTiffHandle, TIFFTAG_SAMPLEFORMAT, sampleformat)
      || !TIFFSetField(pTiffHandle, TIFFTAG_ORIENTATION, orientation)
       )
    {
        TIFFClose(pTiffHandle);
        return false;
    }

    if (compression != COMPRESSION_NONE && !TIFFSetField(pTiffHandle, TIFFTAG_PREDICTOR, predictor) )
    {
        TIFFClose(pTiffHandle);
        return false;
    }

    // row buffer, because TIFFWriteScanline modifies the original data!
    size_t scanlineSize = TIFFScanlineSize(pTiffHandle);
    const size_t buffer_size = bitsPerChannel * channels * 16 * width;

    cv::AutoBuffer<uchar> _buffer(buffer_size);
    uchar* buffer = _buffer;
    uint64 * buffer16 = (uint64*)buffer;//unsigned int16
    if (!buffer)
    {
        TIFFClose(pTiffHandle);
        return false;
    }

    for (int y = 0; y < height; ++y)
    {
        std::memcpy(buffer, img.ptr(y), scanlineSize);
        int writeResult = TIFFWriteScanline(pTiffHandle, buffer16, y, 0);
        if (writeResult != 1)
        {
            TIFFClose(pTiffHandle);
            return false;
        }
    }
    ++frame_number;
    time_stamp += 1/30.0;
    TIFFWriteDirectory(pTiffHandle); // write into the next directory
    return true;
}

bool TiffWriter::writeHdr(const cv::Mat& _img)
{
    cv::Mat img;
    cv::cvtColor(_img, img, COLOR_BGR2XYZ);
    // IMPORTANT: Note the "w8" option here - this is what allows writing to the bigTIFF format
    // possible ('normal' tiff would be just "w")
    if (!(isOpened()))
    	m_tif = TIFFOpen(m_filename.c_str(), "w8");
    if (!m_tif)
    {
        return false;
    }
    TIFFWriteDirectory(pTiffHandle);
    TIFFSetField(m_tif, TIFFTAG_IMAGEWIDTH, img.cols);
    TIFFSetField(m_tif, TIFFTAG_IMAGELENGTH, img.rows);
    TIFFSetField(m_tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(m_tif, TIFFTAG_COMPRESSION, COMPRESSION_SGILOG);
    TIFFSetField(m_tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_LOGLUV);
    TIFFSetField(m_tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(m_tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_FLOAT);
    TIFFSetField(m_tif, TIFFTAG_ROWSPERSTRIP, 1);
    int strip_size = 3 * img.cols;
    float *ptr = const_cast<float*>(img.ptr<float>());
    for (int i = 0; i < img.rows; i++, ptr += strip_size)
    {
        TIFFWriteEncodedStrip(m_tif, i, ptr, strip_size * sizeof(float));
    }
    return true;
}
void TiffWriter::writeTag( WLByteStream& strm, TiffTag tag,
                             TiffFieldType fieldType,
                             int count, int value )
{
    strm.putWord( tag );
    strm.putWord( fieldType );
    strm.putDWord( count );
    strm.putDWord( value );
}

bool TiffWriter::writeSIHdr(const std::string swTag, const std::string imDescTag) {
    auto _swTag = swTag.c_str();
    auto _imDescTag = imDescTag.c_str();
    if ( ( TIFFSetField(m_tif, TIFFTAG_IMAGEDESCRIPTION, _imDescTag) > 0 ) &&
        ( TIFFSetField(m_tif, TIFFTAG_SOFTWARE, _swTag) > 0 ) )
        return true;
    return false;
}

bool TiffWriter::write( const cv::Mat& img, const std::vector<int>& params)
{
	int channels = img.channels();
    int width = img.cols, height = img.rows;
    int depth = img.depth();
    if(img.type() == CV_32FC3)
    {
        return writeHdr(img);
    }
    if (depth != CV_8U && depth != CV_16U && depth != CV_16S)
        return false;
    return writeLibTiff(img, params);
}

bool TiffWriter::isOpened()
{
	if (opened)
		return true;
	else
		return false;
}

bool TiffWriter::open(cv::String outputPath)
{
	if (!(opened))
	{
        // IMPORTANT: Note the "w8" option here - this is what allows writing to the bigTIFF format
        // possible ('normal' tiff would be just "w")
		m_tif = TIFFOpen(outputPath.c_str(), "w8");
		m_filename = outputPath;
		opened = true;
	}
}

bool TiffWriter::close() {
    if ( opened ) {
        TIFFClose(m_tif);
        opened = false;
    }
}

TiffWriter& TiffWriter::operator << (cv::Mat& frame)
{
	if (opened)
	{
		std::vector<int> params;
		write(frame, params);
	}
}
std::string TiffWriter::type2str(int type)
{
	std::string r;
	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth)
	{
		case CV_8U:   r = "8U"; break;
		case CV_8S:   r = "8S"; break;
		case CV_16U:  r = "16U"; break;
		case CV_16S:  r = "16S"; break;
		case CV_32S:  r = "32S"; break;
		case CV_32F:  r = "32F"; break;
		case CV_64F:  r = "64F"; break;
		default:      r = "User"; break;
	}

	r += "C";
	r += (chans+'0');
	return r;
}
} //namespace cv