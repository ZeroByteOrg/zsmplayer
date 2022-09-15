Command line ZSM player. Cross-compiles for Windows using mingw32.

Usage: zsmplayer file1 file2 file3 ...

Known issues:
Synchronization between YM voices and VERA PSG voices may not be perfect depending on how the audio stream startup behaves.

No control over looping behavior.

No validity checks on ZSM files - just blindly tries to play whatever you load.

Opens two separate audio streams; one for each chip. Does not do resampling / mixing in current version.
