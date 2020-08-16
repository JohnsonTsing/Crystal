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
/// @brief  Implementation file initializing the speech synthesis modules for Putonghua (Mandarin) Text-to-Speech (TTS) engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   Jackie (jackiecao@gmail.com)
///   Date:     2007/05/22
///   Changed:  Created
/// - Version:  0.1.1
///   Author:   John (john.zywu@gmail.com)
///   Date:     2007/11/16
///   Changed:  Implemented the framework
///

#include "tts_synthesizer.h"
#include "ttsbase/datamanage/base_moduleapi.h"
#include "ttsputonghua/synth.hts/data_voicedata.h"
#include "ttsputonghua/synth.hts/unitseg_unitsegment.h"
#include "ttsputonghua/synth.hts/psp_prosodypredict.h"
#include "ttsputonghua/synth.hts/hts_synthesize.h"

namespace cst
{
    namespace tts
    {
        extern "C"
        {
            base::CDataManager *CreateDataManager(const base::DataConfig &dataConfig)
            {
                Putonghua::CVoiceData *pDataManager = new Putonghua::CVoiceData();
                if (pDataManager == NULL || !pDataManager->initialize(dataConfig))
                {
                    delete pDataManager;
                    pDataManager = NULL;
                }
                return pDataManager;
            }

            void DeleteDataManager(base::CDataManager *pDataManager)
            {
                delete pDataManager;
            }

            base::CModule *CreateUnitSegment(const base::CDataManager *pDataManager)
            {
                return new Putonghua::CUnitSegment(pDataManager);
            }

            base::CModule *CreateProsodyPredict(const base::CDataManager *pDataManager)
            {
                return new Putonghua::CProsodyPredict(pDataManager);
            }

            base::CModule *CreateSynthesize(const base::CDataManager *pDataManager)
            {
                Putonghua::CHtsSynthesize* pSynthesizer = new Putonghua::CHtsSynthesize(pDataManager);
                return (base::CSynthesize*)pSynthesizer;
            }

            void DeleteModule(base::CModule *pModule)
            {
                delete pModule;
            }

            CST_EXPORT void GetModuleInfo(std::map<std::string, ModuleInfo> &modules)
            {
                modules["CVoiceData"] = ModuleInfo("cst::tts::Putonghua::CVoiceData", CreateDataManager, DeleteDataManager);
                modules["CUnitSegment"] = ModuleInfo("cst::tts::Putonghua::CUnitSegment", CreateUnitSegment, DeleteModule);
                modules["CProsodyPredict"] = ModuleInfo("cst::tts::Putonghua::CProsodyPredict", CreateProsodyPredict, DeleteModule);
                modules["CSynthesize"] = ModuleInfo("cst::tts::Putonghua::CHtsSynthesize", CreateSynthesize, DeleteModule);
            }
        }
    }
}
