USM (CRI Movie 2) Demuxer

usage:
	-o (name) [output folder]
	-k (key) [8-bytes USM key (i.e.2341683D2FDBA6)]
	-m (key) [path to 32-bytes audio mask keyfile]
	-n [don't mask audio data]
	-x [demux audio]
	-v [demux video]
	-i [demux info]
	-c [convert adx to wav instead of demuxing]

****

Mod by mos9527
* multiple streams (if any) will be extracted all at once
* embeded Persona 5 Royal (Switch/PC Release) USM Key
* USM keys are now specifed in 64bits (i.e. no longer divided to hi/low parts)
* added option to NOT mask audio data (see explaination below)

****

Walkthrough : Re-multiplexing USMs from Persona 5 Royal
(Switch/Steam Release)
** Extracting USMs
* The movies are stored in MOVIE_JE.CPK. Extract it w/ https://github.com/Sewer56/CriFsV2Lib
* For P5R, the following switches are used:
	cri_demux_p5r.exe -x -v -n -o . [MOV***.USM]

-x,-v Demuxes Video & Audio (w/ the latter, all available audio tracks are extracted)

-n    **IMPORTANT**. See the explaination below:

Some USM files in P5R uses HCA codec for audio tracks. However,
the demuxer doesn't handle the new encryption scheme CRIWare implemented.

TL;DR, if the TOC listed during extraction has audio files w/ .hca as extension, this switch needes to be there so external tools can handle it themselves.
Otherwise, with other extensions like .avi, this switch is needed because they used the legacy XOR crypto (which the demuxer has)

-o . [OPTIONAL] Outputs extracted content in the root directory.
* Resulting files will ususally have extensions like *.avi (ADPCM for audio,MPEG for video), *.ivf (VP9 video), and *.hca

**Remuxing into common video formats (with FFMpeg)

** Preliminary actions (with *.hca)
* As explained above, HCA tracks needed to be decrypted seperately. Use https://github.com/vgmstream/vgmstream
  to convert them to WAVs first.

* Muxing A/V
  You probably want to come up with something on yourown...
  The example here combines audio/video w/o re-encoding them into a MKV container
  
	ffmpeg -i [audio file] -i [video file] -c:a copy -c:v copy [output].mkv

Original README by bnnm
============================================
CRID USM extractor
v1 by nyaga (https://github.com/Nyagamon)
v2 by bnnm (added new options, xorkey files)
v3 by bnnm (added audio stream selection)

****

Some .USM are encrypted using a 64b key (like HCA) so when demuxing the .adx you get bad audio.

How to decrypt:
- unzip CRID(.usm)Demux Tool v1.01-mod
- find game's USM key, often same as HCA key (see hca_keys.h)
  example, in hex: 006CCC569EB1668D
- call crid_mod.exe with key divided in two to extract .adx
  crid_mod.exe -b 006CCC56 -a 9EB1668D -x Opening.usm
- or flags to extract video, info and stuff:
  crid_mod.exe -b 006CCC56 -a 9EB1668D -i -v Opening.usm

If your .usm has multiple audio streams don't forget to use -s N to extract one by one (manually ATM) or you'll get a garbled .adx

If you don't have the USM key you may still be able to decrypt audio.
The key is used to get a 0x20 xor, and .adx often have long silent (blank) parts, meaning easy keys.

- open .usm in hex editor, look for "@SFA" chunks
- if chunk is bigger than 0x140 try to find repeating 0x20 patterns, that's the key
  (encryption is only applied after 0x140, some chunks are smaller than that)
- if no patterns try other videos, silence/blank is often at the beginning or end of files
- save 0x20 pattern key into key.bin
- call CRID:
  crid_mod.exe -m key.bin Opening.usm

Video uses a slightly different method I don't think you can get as easily.

Needless to say you need to original .usm, can't use vgmtoolbox to demux first.

-- bnnm
