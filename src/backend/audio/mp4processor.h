/*
 *    Copyright (C) 2018
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *
 *    Copyright (C) 2013, 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef MP4PROCESSOR
#define MP4PROCESSOR
/*
 *  Handling superframes for DAB+ and delivering
 *  frames into the ffmpeg or faad decoding library
 */
//
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdint>
#include "CAudio.h"
#include "dab-constants.h"
#include "dab-processor.h"
#include "CFaadDecoder.h"
#include "firecode-checker.h"
#include "reed-solomon.h"
#include "data/pad_decoder.h"
#include "radio-controller.h"

enum class AACAudioMode { Unknown, Mono, Stereo, ParametricStereo};

class mp4Processor : public dabProcessor, public PADDecoderObserver
{
    public:
        mp4Processor(RadioControllerInterface& mr, int16_t bitRate);
        void addtoFrame(uint8_t *v);

        // PADDecoderObserver impl
        void PADChangeDynamicLabel(const DL_STATE& dl);
        void PADChangeSlide(const MOT_FILE& slide);

    private:
        RadioControllerInterface& myRadioInterface;
        bool  processSuperframe(uint8_t frameBytes[], int16_t base);
        void  handle_aacFrame(uint8_t *v,
                              int16_t  frame_length,
                              uint8_t  dacRate,
                              uint8_t  sbrFlag,
                              uint8_t  mpegSurround,
                              uint8_t  aacChannelMode,
                              bool    *error);
        void processPAD(const uint8_t *data);
        int16_t     superFramesize;
        int16_t     blockFillIndex;
        int16_t     blocksInBuffer;
        int16_t     blockCount;
        int16_t     bitRate;
        std::vector<uint8_t> frameBytes;
        uint8_t   **RSMatrix;
        int16_t     RSDims;
        int16_t     au_start[10];
        int32_t     baudRate;

        int32_t     au_count;
        int16_t     au_errors;
        int16_t     errorRate;
        firecode_checker    fc;
        reedSolomon the_rsDecoder;
        std::vector<uint8_t> outVector;
        //  and for the aac decoder
        CFaadDecoder aacDecoder;
        int16_t     frameCount;
        int16_t     successFrames;
        int16_t     frameErrors;
        int16_t     rsErrors;
        int16_t     aacErrors;
        int16_t     aacFrames;
        int16_t     charSet;

        std::string mscFileName;
        struct FILEDeleter{ void operator()(FILE* fd){ if (fd) fclose(fd); }};
        std::unique_ptr<FILE, FILEDeleter> mscFile;

        AACAudioMode aacAudioMode;

        PADDecoder padDecoder;
};

#endif


