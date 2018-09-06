#include "../include/ScanImageTiff.h"

#include <stdio.h>
#include <stdio_ext.h>

#include <limits>

void SITiffHeader::read(TIFF * m_tif, int dirnum)
{
	if ( m_tif )
	{
		if ( dirnum == -1 )
			TIFFSetDirectory(m_tif, dirnum);
		else
		{
			// scrape some big-ish strings out of the header
			char * imdesc;
			if ( TIFFGetField(m_tif, TIFFTAG_IMAGEDESCRIPTION, &imdesc) == 1)
			{
				m_imdesc = imdesc;
			}
			else
				return;
			// get / set frame number and timestamp

			// get / set image size
			int length, width;
			TIFFGetField(m_tif, TIFFTAG_IMAGELENGTH, &length);
			TIFFGetField(m_tif, TIFFTAG_IMAGEWIDTH, &width);
			//
		}
	}
}

void SITiffHeader::versionCheck(TIFF * m_tif)
{
	if ( m_tif )
	{
		m_imdesc = getImageDescTag(m_tif, 0);
		if ( ! grabStr(m_imdesc, "Frame Number =").empty() ) // old
		{
			version = 0;
			channelSaved = "scanimage.SI5.channelsSave =";
			channelLUT = "scanimage.SI5.chan1LUT =";
			channelOffsets = "scanimage.SI5.channelOffsets =";
			frameString = "Frame Number =";
			frameTimeStamp = "Frame Timestamp(s) =";
		}
		else if ( ! grabStr(m_imdesc, "frameNumbers =").empty() ) // new
		{
			version = 1;
			channelSaved = "SI.hChannels.channelSave =";
			channelLUT = "SI.hChannels.channelLUT =";
			channelOffsets = "SI.hChannels.channelOffset =";
			channelNames = "SI.hChannels.channelName =";
			frameString = "frameNumbers =";
			frameTimeStamp = "frameTimestamps_sec =";
		}

		uint32 length;
		uint32 width;
		TIFFGetField(m_tif, TIFFTAG_IMAGELENGTH, &length);
		TIFFGetField(m_tif, TIFFTAG_IMAGEWIDTH, &width);
		m_parent->setImageSize(length, width);
	}
}

std::string SITiffHeader::getSoftwareTag(TIFF * m_tif, unsigned int dirnum)
{
	if ( m_tif )
	{
		TIFFSetDirectory(m_tif, dirnum);
		if ( version == 0 )
		{
			// with older versions the information for channels live
			// in a different tag (ImageDescription) and some of that
			// information doesn't even exist so has to be inferred...
			std::string imdesc = getImageDescTag(m_tif, dirnum);
			if ( ! imdesc.empty() )
			{
				std::string chanSave = grabStr(imdesc, channelSaved);
				chanSaved[0] = 1;

				std::string chanLUTs = grabStr(imdesc, channelLUT);
				parseChannelLUT(chanLUTs);

				std::string chanOffsets = grabStr(imdesc, channelOffsets);
				parseChannelOffsets(chanOffsets);
				return imdesc;
			}
			else
				return std::string();

		}
		else if ( version == 1 )
		{
			char * swTag;
			if ( TIFFGetField(m_tif, TIFFTAG_SOFTWARE, & swTag) == 1 )
			{
				m_swTag = swTag;
				std::string chanNames = grabStr(m_swTag, channelNames);
				std::string chanLUTs = grabStr(m_swTag, channelLUT);
				std::string chanSave = grabStr(m_swTag, channelSaved);
				std::string chanOffsets = grabStr(m_swTag, channelOffsets);
				parseChannelLUT(chanLUTs);
				parseChannelOffsets(chanOffsets);
				parseSavedChannels(chanSave);
				return m_swTag;
			}
			else
				return std::string();
		}
		else
			return std::string();
	}
	return std::string();
}

std::string SITiffHeader::getImageDescTag(TIFF * m_tif, unsigned int dirnum)
{
	if ( m_tif )
	{
		TIFFSetDirectory(m_tif, dirnum);
		char * imdesc;
		if ( TIFFGetField(m_tif, TIFFTAG_IMAGEDESCRIPTION, &imdesc) == 1)
		{
			m_imdesc = imdesc;
			return m_imdesc;
		}
		else {
			std::cout << "Image description tag empty\n";
			return std::string();
		}
	}
	std::cout << "m_tif not valid\n";
	return std::string();
}

std::string SITiffHeader::grabStr(const std::string source, const std::string & target)
{
	std::size_t start = source.find(target);
	if ( start != std::string::npos )
	{
		std::string fn = source.substr(start + target.length());
		std::size_t newline = fn.find('\n');
		if ( newline != std::string::npos )
		{
			fn = fn.substr(0, newline);
			return fn;
		}
		else
			return std::string();
	}
	else
		return std::string();
}

unsigned int SITiffHeader::getSizePerDir(TIFF * m_tif, unsigned int dirnum)
{
	if ( m_tif )
	{
		TIFFSetDirectory(m_tif, dirnum);
		uint32 length;
		uint32 width;
		TIFFGetField(m_tif, TIFFTAG_IMAGELENGTH, &length);
		TIFFGetField(m_tif, TIFFTAG_IMAGEWIDTH, &width);
		// hard-code the sample format (signed int) as SI seems to only
		// use this which is of size 4
		return (length * width * 4);
	}
	else
		return 0;
}

void SITiffHeader::printHeader(TIFF * m_tif, int framenum)
{
	if ( m_tif )
	{
		TIFFSetDirectory(m_tif, framenum);
		TIFFPrintDirectory(m_tif, stdout, 0);
	}
}

int SITiffHeader::getNumFrames(TIFF * m_tif, int idx, int & count)
{
	if ( m_tif )
	{
		TIFFSetDirectory(m_tif, idx);
		if ( TIFFReadDirectory(m_tif) == 1 )
		{
			++count;
			return 0;
		}
		else
			TIFFSetDirectory(m_tif, 0);
	}
	return 1;
}

int SITiffHeader::countDirectories(TIFF * m_tif, int & count)
{
	if ( m_tif )
	{
		do {
			++count;
		}
		while ( TIFFReadDirectory(m_tif) == 1 );
	}
	return 1;
}

int SITiffHeader::scrapeHeaders(TIFF * m_tif, int & count)
{
	if ( m_tif )
	{
		if ( TIFFReadDirectory(m_tif) == 1 )
		{
			std::string imdescTag = getImageDescTag(m_tif, count);
			if ( ! imdescTag.empty() )
			{
				std::string ts_str = grabStr(imdescTag, frameTimeStamp);
				if ( ! ts_str.empty() )// sometimes headers are corrupted esp. at EOF
				{
					double ts = std::stof(ts_str);
					m_timestamps.push_back(ts);
					++count;
					return 0;
				}
				else
					return 1;
			}
			else
				return 1;
		}
		else
			return 1;
	}
	return 1;
}

void SITiffHeader::parseChannelLUT(std::string LUT)
{
	LUT.pop_back();
	LUT = LUT.substr(2);
	auto LUT_split = split(LUT, ' ');
	for (auto & x : LUT_split)
	{
		if (x.find('[') != std::string::npos)
			x = x.substr(1);
		if (x.find(']') != std::string::npos)
			x.pop_back();
	}
	int count = 1;
	for (unsigned int i = 0; i < LUT_split.size(); i+=2)
	{
		chanLUT[count] = std::make_pair<int,int>(std::stoi(LUT_split[i]), std::stoi(LUT_split[i+1]));
		++count;
	}
}

void SITiffHeader::parseChannelOffsets(std::string offsets)
{
	offsets.pop_back();
	offsets = offsets.substr(2);
	auto offsets_split = split(offsets, ' ');
	int count = 1;
	for ( auto & x : offsets_split)
	{
		if (x.find('[') != std::string::npos)
			x = x.substr(1);
		if (x.find(']') != std::string::npos)
			x.pop_back();
		chanOffs[count] = std::stoi(x);
		++count;
	}
}

void SITiffHeader::parseSavedChannels(std::string savedchans)
{
	// in case only a single channel has been saved
	if ( savedchans.size() == 2 ) {
		chanSaved[0] = std::stoi(savedchans.substr(1));
	}
	else {
		savedchans.pop_back();
		savedchans = savedchans.substr(2);
		auto savedchans_split = split(savedchans, ';');
		int count = 0;
		for (auto & x: savedchans_split)
		{
			chanSaved[count] = std::stoi(x);
			++count;
		}
	}
}

/* -----------------------------------------------------------
class SITiffReader
------------------------------------------------------------*/
SITiffReader::~SITiffReader()
{
	if ( headerdata )
		delete headerdata;
}
bool SITiffReader::open()
{
	m_tif = TIFFOpen(m_filename.c_str(), "r");
	if ( m_tif )
	{
		std::cout << "Opening tif file: " << m_filename << std::endl;
		headerdata = new SITiffHeader{this};
		std::cout << "Checking version of tiff file...\n";
		headerdata->versionCheck(m_tif);
		std::cout << "Version is " << headerdata->getVersion() << std::endl;
		isopened = true;
		return true;
	}
	return false;
}

bool SITiffReader::readheader()
{
	if ( m_tif )
	{
		std::string softwareTag = headerdata->getSoftwareTag(m_tif);
		return true;
	}
	return false;
}

std::vector<double> SITiffReader::getAllTimeStamps() {
	if ( m_tif ) {
		std::cout << "Starting scraping timestamps..." << std::endl;
		TIFFSetDirectory(m_tif, 0);
		int count = 1;
		do {}
		while ( headerdata->scrapeHeaders(m_tif, count) == 0 );
		std::cout << "Finished scraping timestamps..." << std::endl;
		return headerdata->getTimeStamps();
	}
}

void SITiffReader::getFrameNumAndTimeStamp(const unsigned int dirnum, unsigned int & framenum, double & timestamp)
{
	if ( m_tif )
	{
		// strings to grab from the header
		const std::string frameNumberString = headerdata->getFrameNumberString();
		const std::string frameTimeStampString = headerdata->getFrameTimeStampString();
		std::string imdescTag = headerdata->getImageDescTag(m_tif, dirnum);
		int version = getVersion();
		std::string frameN;
		std::string ts;
		frameN = headerdata->grabStr(imdescTag, frameNumberString);
		ts = headerdata->grabStr(imdescTag, frameTimeStampString);
		framenum = std::stoi(frameN);
		timestamp = std::stof(ts);
	}
}
bool SITiffReader::release()
{
	if ( m_tif )
	{
		TIFFClose(m_tif);
		return true;
	}
	else
		return false;
}
cv::Mat SITiffReader::readframe(int framedir)
{
	if ( m_tif )
	{
		cv::Mat frame;
		int framenum = framedir;
		TIFFSetDirectory(m_tif, framenum);
		uint32 w = 0, h = 0;
		uint16 photometric = 0;
		if( TIFFGetField( m_tif, TIFFTAG_IMAGEWIDTH, &w ) && // normally = 512
            TIFFGetField( m_tif, TIFFTAG_IMAGELENGTH, &h ) && // normally = 512
            TIFFGetField( m_tif, TIFFTAG_PHOTOMETRIC, &photometric )) // photometric = 1 (min-is-black)
        {
        	m_imagewidth = w;
            m_imageheight = h;

        	uint16 bpp=8, ncn = photometric > 1 ? 3 : 1;
        	TIFFGetField( m_tif, TIFFTAG_BITSPERSAMPLE, &bpp ); // = 16
            TIFFGetField( m_tif, TIFFTAG_SAMPLESPERPIXEL, &ncn ); // = 1
            int is_tiled = TIFFIsTiled(m_tif); // 0 ie false, which means the data is organised in strips
            uint32 tile_height0 = 0, tile_width0 = m_imagewidth;
            TIFFGetField(m_tif, TIFFTAG_ROWSPERSTRIP, &tile_height0);

            if( (!is_tiled) ||
	            (is_tiled &&
	            TIFFGetField( m_tif, TIFFTAG_TILEWIDTH, &tile_width0) &&
	            TIFFGetField( m_tif, TIFFTAG_TILELENGTH, &tile_height0 )))
	        {
	            if(!is_tiled)
	                TIFFGetField( m_tif, TIFFTAG_ROWSPERSTRIP, &tile_height0 );

	            if( tile_width0 <= 0 )
	                tile_width0 = m_imagewidth;

	            if( tile_height0 <= 0 ||
	               (!is_tiled && tile_height0 == std::numeric_limits<uint32>::max()) )
	                tile_height0 = m_imageheight;

	            const size_t buffer_size = bpp * ncn * tile_height0 * tile_width0;

	            cv::AutoBuffer<uchar> _buffer( buffer_size );
	            uchar* buffer = _buffer;
	            ushort* buffer16 = (ushort*)buffer;
	            int tileidx = 0;



	            // ********* return frame created here ***********

	            frame = cv::Mat(h, w, cv_matrix_type);
	            uchar * data = frame.ptr();

	            for (int y = 0; y < m_imageheight; y+=tile_height0, data += frame.step*tile_height0)
	            {
	            	int tile_height = tile_height0;


	            	if( y + tile_height > m_imageheight )
	                    tile_height = m_imageheight - y;
	                // tile_height is always equal to 8

	                for(int x = 0; x < m_imagewidth; x += tile_width0, tileidx++)
	                {
	                	int tile_width = tile_width0, ok;

	                    if( x + tile_width > m_imagewidth )
	                        tile_width = m_imagewidth - x;
	                    // I've cut out lots of bpp testing etc here
	                    // tileidx goes from 0 to 63
	                    ok = (int)TIFFReadEncodedStrip(m_tif, tileidx, (uint32*)buffer, buffer_size ) >= 0;
	                    if ( !ok )
	                    {
	                    	close();
	                    	return cv::Mat();
	                    }
	                    for(int i = 0; i < tile_height; ++i)
	                    {
	                    	std::memcpy((ushort*)(data + frame.step*i)+x,
	                                    buffer16 + i*tile_width0*ncn,
	                                    tile_width*sizeof(buffer16[0]));
	                    }
	                }
	            }

	        }
        }
        return frame;
	}
	return cv::Mat();
}

bool SITiffReader::close()
{
	TIFFClose(m_tif);
	isopened = false;
	if ( headerdata )
		delete headerdata;
	return true;
}