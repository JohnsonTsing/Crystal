//
//  Crystal Text-to-Speech Engine
//
//  Copyright (c) 2007 THU-CUHK Joint Research Center for
//  Media Sciences, Technologies and Systems. All rights reserved.
//
//  http://mjrc.sz.tsinghua.edu.cn
//
//  Redistribution and use in source and binary forms, with or without
//  modification, is not allowed, unless a valid written license is
//  granted by THU-CUHK Joint Research Center.
//
//  THU-CUHK Joint Research Center has the rights to create, modify,
//  copy, compile, remove, rename, explain and deliver the source codes.
//

///
/// @file
///
/// @brief  Implementation file for prosodic prediction module of Putonghua TTS engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/23
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   Yongxin Wang (fefe.wyx@gmail.com)
///   Data:     2007/06/13
///   Changed:  implemented neutral prosody prediction
/// - Version:  0.2.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2008/07/29
///   Changed:  Re-implemented using the new architecture with internal representation
/// - Version:  0.2.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2009/01/20
///   Changed:  Added processing of UTF-16 non-BMP characters in segmentToUnits
///

#include "data_voicedata.h"
#include "psp_prosodypredict.h"

namespace cst
{
    namespace tts
    {
        namespace Putonghua
        {
            int CProsodyPredict::predictProsody(std::vector<CUnitItem> &sentenceInfo)
            {
                return ERROR_SUCCESS;
            }

        }//namespace Putonghua
    }
}
