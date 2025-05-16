//
// Class to read data from zip file
// Uses zlib to unzip data from file
// 
// Author: Alex V. Boreskoff
//
// Last modified: 15/11/2002
//

#include	"ZipFileSystem.h"
#include	"Log.h"
#include	"zlib.h"
#include	"Data.h"

#define	LOCAL_ZIP_SIGNATURE		0x04034B50
#define	CENTRAL_ZIP_SIGNATURE	0x02014B50
#define	END_CENTRAL_SIGNATURE	0x06054B50
#define	EXTD_LOCAL_SIGNATURE	0x08074B50

#define DEF_WBITS	15					// Default LZ77 window size
#define ZIP_STORE	0					// 'STORED' method id
#define ZIP_DEFLATE	8					// 'DEFLATE' method id


#pragma pack (push, 1)					// save current pack, set 1-byte packing

struct	ZipLocalFileHeader
{
	unsigned long	signature;
	unsigned short	versionNeededToExtract;
	unsigned short	generalPurposeBitFlag;
	unsigned short	compressionMethod;
	unsigned short	lastModFileTime;
	unsigned short	lastModFileDate;
	unsigned long	crc32;
	long			compressedSize;
	long			uncompressedSize;
	unsigned short	filenameLength;
	unsigned short	extraFieldLength;
};

struct	ZipDataDescriptor
{
	unsigned long	crc32;
	unsigned long	compressedSize;
	unsigned long	uncompressedSize;
};

struct	ZipCentralHeader				// of central header
{
	unsigned long	signature;
	unsigned short	versionMadeBy;
	unsigned short	versionNeededToExtract;
	unsigned short	generalPurposeBitFlag;
	unsigned short	compressionMethod;
	unsigned short	lastModFileTime;
	unsigned short	lastModFileDate;
	unsigned long	crc32;
	long			compressedSize;
	long			uncompressedSize;
	unsigned short	filenameLength;
	unsigned short	extraFieldLength;
	unsigned short  commentLength;
	unsigned short	diskNumberStart;
	unsigned short	internalFileAttibutes;
	unsigned long	externalFileAttributes;
	long			relativeLocalHeaderOffset;
};

struct	ZipEndOfCentralDir
{
	unsigned long	signature;
	unsigned short	diskNo;
	unsigned short	centralDirDiskNo;
	unsigned short	numEntriesOnDisk;
	unsigned short	numEntries;
	unsigned long	centralDirSize;
	long			centralDirOffset;
	unsigned short	commentLength;
};

#pragma	pack (pop)

#define	CHUNK_SIZE	1024					// chunk size for reading zip file from end

/////////////////// Local class to keep info about single file in zip ///////////////////

class	ZipFileEntry : public Object
{
private:
	String				fileName;						// name of the file
	ZipCentralHeader	hdr;
public:
	ZipFileEntry ( const char * theName, const ZipCentralHeader& theHeader ) : Object ( "" ), fileName ( theName )
	{
		hdr       = theHeader;
		metaClass = &classInstance;
	}

	const String& getFileName () const
	{
		return fileName;
	}

	const ZipCentralHeader& getHeader () const
	{
		return hdr;
	}

	static	MetaClass	classInstance;
};

//////////////////////////////// ZipFileSystem methods //////////////////////////////////

MetaClass	ZipFileEntry  :: classInstance ( "ZipFileEntry",  &Object :: classInstance );
MetaClass	ZipFileSystem :: classInstance ( "ZipFileSystem", &ResourceSource :: classInstance );

ZipFileSystem :: ZipFileSystem ( const String& zipName ) : ResourceSource ( "ZipFileSystem" ), fileName ( zipName )
{
	metaClass = &classInstance;

	int	file = open ( fileName, O_BINARY | O_RDONLY );

	if ( file == -1 )
		return;

	broken = false;

	readDirectory ( file );
	close         ( file );

	(*sysLog) << "Added zip source: " << zipName << logEndl;
}

Data * ZipFileSystem :: getFile  ( const String& name )
{
	for ( Array :: Iterator it = dir.getIterator (); !it.end (); ++it )
	{
		const ZipFileEntry * entry = (const ZipFileEntry *) it.value ();

		if ( entry -> getFileName ().caseInsensitiveCompare ( name ) == 0 )		// found name
			return readEntry ( entry -> getHeader () );
	}

	return NULL;
}

void	ZipFileSystem :: readDirectory ( int file )
{
	ZipCentralHeader	hdr;
	char				buf [CHUNK_SIZE];
	size_t				step = sizeof ( ZipEndOfCentralDir );

	lseek ( file, 0, SEEK_END );

	size_t	offs    = tell ( file );
	size_t	minOffs = offs - (65535 + step );
	size_t	pos;

	if ( minOffs < 0 )
		minOffs = 0;

	while ( offs > minOffs )
	{
		if ( offs > sizeof ( buf ) - step )
			offs -= sizeof ( buf ) - step;
		else
			offs = 0;

		lseek ( file, offs, SEEK_SET );

		pos = read ( file, buf, sizeof ( buf ) );

		if ( pos < step )
			continue;

		for ( int i = pos - step; i > 0; i-- )
			if ( *(unsigned long *) &buf [i] == END_CENTRAL_SIGNATURE )
			{
								// load end of central dir record
				const ZipEndOfCentralDir * dirEndPtr = (const ZipEndOfCentralDir *) &buf [i];

				if ( lseek ( file, dirEndPtr -> centralDirOffset, SEEK_SET ) != dirEndPtr -> centralDirOffset )
				{
					broken = true;

					return;
				}
								// now read central dir structure
				for ( ; ; )
				{
					if ( read ( file, &hdr, sizeof ( hdr ) ) != sizeof ( hdr ) )
						return;				// finished

					if ( hdr.signature != CENTRAL_ZIP_SIGNATURE )
					{
						broken = true;

						return;
					}

							// now read the name
					if ( hdr.filenameLength >= sizeof ( buf ) )
					{
						broken = true;

						return;
					}

					read ( file, buf, hdr.filenameLength );

					buf [hdr.filenameLength] = '\0';

					dir.insertNoRetain ( new ZipFileEntry ( buf, hdr ) );
				}
			}
	}
}

Data * ZipFileSystem :: readEntry ( const ZipCentralHeader& hdr )
{
	int	size;
	int	file = open ( fileName.c_str (), O_BINARY | O_RDONLY );

	if ( file == -1 )
		return NULL;

	if ( lseek ( file, hdr.relativeLocalHeaderOffset, SEEK_SET ) != hdr.relativeLocalHeaderOffset )
	{
		close ( file );

		return NULL;
	}

	ZipLocalFileHeader	localHdr;

	if ( read ( file, &localHdr, sizeof ( localHdr ) ) != sizeof ( localHdr ) )
	{
		close ( file );

		return NULL;
	}

	if ( localHdr.signature != LOCAL_ZIP_SIGNATURE )
	{
		close ( file );

		return NULL;
	}

	lseek ( file, localHdr.filenameLength + localHdr.extraFieldLength, SEEK_CUR );

	char	inBuffer [2048];
	size_t	bytesLeft = hdr.compressedSize;
	size_t	blockSize;
	void  * buf = malloc ( hdr.uncompressedSize + 1 );

	switch ( hdr.compressionMethod )
	{
		case ZIP_STORE:
			if ( read ( file, buf, hdr.compressedSize ) != hdr.compressedSize )
			{
				free  ( buf );
				close ( file );

				return NULL;
			}

			break;

		case ZIP_DEFLATE:
			z_stream	zs;
			int			err;

			zs.next_out  = (unsigned char *)buf;
			zs.avail_out = hdr.uncompressedSize;
			zs.zalloc    = NULL;
			zs.zfree     = NULL;

			if ( inflateInit2 ( &zs, -DEF_WBITS ) != Z_OK )
			{
				free  ( buf );
				close ( file );

				return NULL;
			}

			while ( bytesLeft > 0 )
			{
				blockSize   = bytesLeft > sizeof ( inBuffer ) ? size = sizeof ( inBuffer ) : bytesLeft;
				zs.next_in  = (unsigned char *)inBuffer;
				zs.avail_in = read ( file, inBuffer, blockSize );

				err        = inflate ( &zs, bytesLeft > blockSize ? Z_PARTIAL_FLUSH : Z_FINISH );
				bytesLeft -= blockSize;
			}

			inflateEnd ( &zs );

			if ( err != Z_STREAM_END )
			{
				free  ( buf );
				close ( file );

				return NULL;
			}

			break;

		default:
			free  ( buf );
			close ( file );

			return NULL;
	}

	close ( file );

	size = hdr.uncompressedSize;

	return new Data ( "", buf, size );
}
