Let's have fun summarizing a RFC (fml)
================================

Xiph.org libraries
------------------
- libogg -> read data from ogg file
- libvorbis -> decode vorbis
- libtheora -> decode theora

Ogg file format
---------------
- *physical bistream* = result of an Ogg encapsulation
- encapsulates one or several *logical bitstreams*
- *logical bitsream* = sequence of *packets* (mean something for the decoder only)
- each *packet* is divided in *segments*, and contains only one type of data
- each *page* contains a header and several *segments* of the same packet (255 bytes each)
- *packets* can be on one or several pages
- *logical bitstream* starts and ends with a special page (bos, eos)
- pages =~ link between logical and physical ? Not understood a word of it

- *granule positions* = unitless position markers
- *media mapping* means encapsulating a type of data for stream-based storage
(for example OggVorbis for audio data, introduces 2 additional headers)
=> link granule positions to time, here stored in chronological order

logical bitstream with packet boundaries
	-----------------------------------------------------------------
	> |       packet_1             | packet_2         | packet_3 |  <
	-----------------------------------------------------------------

segmentation (logically only)	|
								v

		packet_1 (5 segments)          packet_2 (4 segs)    p_3 (2 segs)
	------------------------------ -------------------- ------------
	..  |seg_1|seg_2|seg_3|seg_4|s_5 | |seg_1|seg_2|seg_3|| |seg_1|s_2 | ..
	------------------------------ -------------------- ------------

page encapsulation	| 
					v

	page_1 (packet_1 data)   page_2 (pket_1 data)   page_3 (packet_2 data)
	------------------------  ----------------  ------------------------
	|H|------------------- |  |H|----------- |  |H|------------------- |
	|D||seg_1|seg_2|seg_3| |  |D|seg_4|s_5 | |  |D||seg_1|seg_2|seg_3| | ...
	|R|------------------- |  |R|----------- |  |R|------------------- |
	------------------------  ----------------  ------------------------

Ogg Vorbis Theora
=================

- Logical format :
	- 2 streams (Vorbis & Theora), each split in packets containing the raw data
	- each stream has a streamstate (keep track of the data read)
	- need a decoder of this stream to use the data
- Physical format :
	- split into pages of data, each one containing some part of the data for one stream
	- process = read pages and insert them with the streamstate they belong to (with pagein)
	- once a packet is complete (packetout indicates if a packet is available), we can decode it

Theora
------
- 3 header packets (info, comment & setup) => in a th_dec_ctx object
(holds information that the decoder requires to keep track of the decoding process)
- when you have a ogg_packet, th_decode_headerin tries to decode a header packet : if success, it's a theora stream

Vorbis
------
Seems very similar


Idea of this program
====================
Ensivideo : 
|--1 thread for Theora : run oggstream.theoraStreamReader
|  |--read all the pages, add them to his streamstate
|  |--when a packet is complete, call stream_common.decodeAllHeaders
|  |  |
|  |  |--if the packet is data and the right type, 1 thread for GUI : run ensitheora.draw2SDL
|  |     |--synchro.attendreTailleFenetre /1/
|  |     |--create window and textures
|  |     |--synchro.signalerFenetreEtTexturePrete /4/
|  |     |--synchro.debutConsommerTexture /7/
|  |     |--print textures while there's no end event
|  |     |--synchro.finConsommerTexture /8/
|  |
|  |--call ensitheora.theora2SDL
|     |--synchro.envoiTailleFenetre /2/
|     |--synchro.attendreFenetreTexture /3/
|     |--synchro.debutDeposerTexture /5/
|     |--prepare the texture to be displayed
|     |--synchro.finDeposerTexture /6/
|
|
|
|--1 thread for Vorbis : run oggstream.vorbisStreamReader
   |--exact same things

