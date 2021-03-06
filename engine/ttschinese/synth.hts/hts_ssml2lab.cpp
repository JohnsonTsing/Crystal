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
/// @brief  Implementation file for SSML to LAB file conversion for HMM based speech synthesis of Putonghua TTS engine
///
/// <b>History:</b>
/// - Version:  0.1.0
///   Author:   John (john.zywu@gmail.com)
///   Date:     2014/12/10
///   Changed:  Created
///


#include "data_voicedata.h"
#include "hts_ssml2lab.h"
#include "utils/utl_regexp.h"
#include "ttsputonghua/utility/utl_pinyin.h"
#include <sstream>

namespace cst
{
    namespace tts
    {
        namespace Putonghua
        {
            int CSSML2Lab::buildLabInfo(std::vector<CUnitItem> &inSenUnits, std::vector<CSegInfo> &outSenSegs)
            {
                // build the context information for a sentence
                std::vector<CUnitInfo> senUnitInfo;
                int flag = buildContext(inSenUnits, senUnitInfo);
                if (flag != ERROR_SUCCESS)
                    return flag;

                // build LAB formated information
                flag = buildLabInfo(senUnitInfo, outSenSegs);
                if (flag != ERROR_SUCCESS)
                    return flag;

                return ERROR_SUCCESS;
            }

            const std::string &CSSML2Lab::CSegInfo::asLabel() const
            {
                static std::string label;
                std::ostringstream fout;
                print(fout);
                label = fout.str();
                return label;
            }

            void CSSML2Lab::CSegInfo::print(std::ostream &fp) const
            {
                static const char* strPattern = "p1^p2-p3+p4=p5@p6_p7/A:a1_0_a3/B:b1-0-b3@b4-b5&b6-b7#b8-b9$0-0!0-0;b14-b15|b16/C:c1+0+c3/D:X_d2/E:X+e2@e3+e4&e5+e6#0+0/F:X_f2/G:g1_g2/H:h1=h2^h3=h4|X/I:i1=i2/K:k1#k2!0@0/M:0+0-0^0/N:n1";

                const char* ptr = strPattern;
                std::string pprPhoneme = preSeg==NULL ? "X" : (preSeg->preSeg==NULL ? "X" : preSeg->preSeg->segPhoneme);
                std::string prePhoneme = preSeg==NULL ? "X" : preSeg->segPhoneme;
                std::string nxtPhoneme = nxtSeg==NULL ? "X" : nxtSeg->segPhoneme;
                std::string nntPhoneme = nxtSeg==NULL ? "X" : (nxtSeg->nxtSeg==NULL ? "X" : nxtSeg->nxtSeg->segPhoneme);
                int preSylTone   = preSyl==NULL ? 0 : preSyl->sylTone;
                int nxtSylTone   = nxtSyl==NULL ? 0 : nxtSyl->sylTone;
                int preSylSegNum = preSyl==NULL ? 0 : preSyl->sylSegNumber;
                int nxtSylSegNum = nxtSyl==NULL ? 0 : nxtSyl->sylSegNumber;
                int prePWdSylNum = prePWd==NULL ? 0 : prePWd->pwdSylNumber;
                int nxtPWdSylNum = nxtPWd==NULL ? 0 : nxtPWd->pwdSylNumber;
                int prePPhSylNum = prePPh==NULL ? 0 : prePPh->pphSylNumber;
                int nxtPPhSylNum = nxtPPh==NULL ? 0 : nxtPPh->pphSylNumber;
                int prePPhPWdNum = prePPh==NULL ? 0 : prePPh->pphPWdNumber;
                int nxtPPhPWdNum = nxtPPh==NULL ? 0 : nxtPPh->pphPWdNumber;

                fp << begTime << "\t";
                fp << endTime << "\t";
                fp << pprPhoneme;           ptr+=2; fp << *ptr++;                               // p1: LL-Seg ^
                fp << prePhoneme;           ptr+=2; fp << *ptr++;                               // p2: L-Seg -
                fp << segPhoneme;           ptr+=2; fp << *ptr++;                               // p3: C-Seg +
                fp << nxtPhoneme;           ptr+=2; fp << *ptr++;                               // p4: R-Seg =
                fp << nntPhoneme;           ptr+=2; fp << *ptr++;                               // p5: RR-Seg @
                fp << segPos2SylHead;       ptr+=2; fp << *ptr++;                               // p6: Seg_Fw _
                fp << segPos2SylTail;       ptr+=2; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // p7: Seg_Bw /A:
                fp << preSylTone;           ptr+=2; fp << *ptr++;                               // a1: L-Syl_tone _
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 _
                fp << preSylSegNum;         ptr+=2; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // a3: L-Syl_Num-Segs /B:
                fp << sylTone;              ptr+=2; fp << *ptr++;                               // b1: C-Syl_tone -
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 -
                fp << sylSegNumber;         ptr+=2; fp << *ptr++;                               // b3: C-Syl_Num-Segs @
                fp << sylPos2PWdHead;       ptr+=2; fp << *ptr++;                               // b4: Pos_C-Syl_in_C-Word(Fw) -
                fp << sylPos2PWdTail;       ptr+=2; fp << *ptr++;                               // b5: Pos_C-Syl_in_C-Word(Bw) &
                fp << sylPos2PPhHead;       ptr+=2; fp << *ptr++;                               // b6: Pos_C-Syl_in_C-Phrase(Fw) -
                fp << sylPos2PPhTail;       ptr+=2; fp << *ptr++;                               // b7: Pos_C-Syl_in_C-Phrase(Bw) #
                fp << sylPos2IPhHead;       ptr+=2; fp << *ptr++;                               // b8: Pos_C-Syl_in_C-BreathGroup(Fw) -
                fp << sylPos2IPhTail;       ptr+=2; fp << *ptr++;                               // b9: Pos_C-Syl_in_C-BreathGroup(Bw) $
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 -
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 !
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 -
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 ;
                fp << sylPreBoundaryType;   ptr+=3; fp << *ptr++;                               // b14: L-Syl_Type_Boundary -
                fp << sylNxtBoundaryType;   ptr+=3; fp << *ptr++;                               // b15: R-Syl_Type_Boundary |
                fp << sylFinal;             ptr+=3; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // b16: C-Syl_Final /C:
                fp << nxtSylTone;           ptr+=2; fp << *ptr++;                               // c1: R-Syl_tone +
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 +
                fp << nxtSylSegNum;         ptr+=2; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // c3: R-Syl_Num-Segs /D:
                fp << "X";                  ptr+=1; fp << *ptr++;                               // X _
                fp << prePWdSylNum;         ptr+=2; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // d2: L-Word_Num-Syls /E:
                fp << "X";                  ptr+=1; fp << *ptr++;                               // X +
                fp << pwdSylNumber;         ptr+=2; fp << *ptr++;                               // e2: C-Word_Num-Syls @
                fp << pwdPos2PPhHead;       ptr+=2; fp << *ptr++;                               // e3: Pos_C-Word_in_C-Phrase(Fw) +
                fp << pwdPos2PPhTail;       ptr+=2; fp << *ptr++;                               // e4: Pos_C-Word_in_C-Phrase(Bw) &
                fp << pwdPos2IPhHead;       ptr+=2; fp << *ptr++;                               // e5: Pos_C-Word_in_C-BreathGroup(Fw) +
                fp << pwdPos2IPhTail;       ptr+=2; fp << *ptr++;                               // e6: Pos_C-Word_in_C-BreathGroup(Bw) #
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 +
                fp << 0;                    ptr+=1; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // 0 /F:
                fp << "X";                  ptr+=1; fp << *ptr++;                               // X _
                fp << nxtPWdSylNum;         ptr+=2; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // f2: R-Word_Num-Syls /G:
                fp << prePPhSylNum;         ptr+=2; fp << *ptr++;                               // g1: L-Phrase_Num-Syls _
                fp << prePPhPWdNum;         ptr+=2; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // g2: L-Phrase_Num-Words /H:
                fp << pphSylNumber;         ptr+=2; fp << *ptr++;                               // h1: C-Phrase_Num-Syls =
                fp << pphPWdNumber;         ptr+=2; fp << *ptr++;                               // h2: C-Phrase_Num-Words ^
                fp << pphPos2IPhHead;       ptr+=2; fp << *ptr++;                               // h3: Pos_C-Phrase_in_BreathGroup(Fw) =
                fp << pphPos2IPhTail;       ptr+=2; fp << *ptr++;                               // h4: Pos_C-Phrase_in_BreathGroup(Bw) |
                fp << "X";                  ptr+=1; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // X /I:
                fp << nxtPPhSylNum;         ptr+=2; fp << *ptr++;                               // i1: R-Phrase_Num-Syls =
                fp << nxtPPhPWdNum;         ptr+=2; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // i2: R-Phrase_Num_words /K:
                fp << iphPWdNumber;         ptr+=2; fp << *ptr++;                               // k1: Num-Words_in_Breath #
                fp << iphPPhNumber;         ptr+=2; fp << *ptr++;                               // k2: Num-Phrases_in_Breath !
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 @
                fp << 0;                    ptr+=1; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // 0 /M:
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 +
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 -
                fp << 0;                    ptr+=1; fp << *ptr++;                               // 0 ^
                fp << 0;                    ptr+=1; fp << *ptr++; fp << *ptr++; fp << *ptr++;   // 0 /N:
                fp << iphIntonationType;    ptr+=2;                                             // n1: Type_BreathGroups
                fp << std::endl;
            }

            void CSSML2Lab::CSegInfo::clear()
            {
                pUnit = NULL;
                preSeg = nxtSeg = preSyl = nxtSyl = prePWd = nxtPWd = prePPh = nxtPPh = NULL;
                begTime = endTime = 0;
                segPhoneme = "";
                segPos2SylHead = segPos2SylTail = 0;
                sylPhoneme = sylFinal = "";
                sylSegs.clear();
                sylTone = 0;
                sylSegNumber = 0;
                sylPos2PWdHead = sylPos2PWdTail = sylPos2PPhHead = sylPos2PPhTail = sylPos2IPhHead = sylPos2IPhTail = 0;
                sylPreBoundaryType = sylNxtBoundaryType = 0;
                pwdSylNumber = 0;
                pwdPos2PPhHead = pwdPos2PPhTail = pwdPos2IPhHead = pwdPos2IPhTail = 0;
                pphSylNumber = pphPWdNumber = 0;
                pphPos2IPhHead = pphPos2IPhTail = 0;
                iphPWdNumber = iphPPhNumber = 0;
                iphIntonationType = 0;
            }

            void CSSML2Lab::CUnitInfo::clear()
            {
                pUnit = NULL;
                preSyl = nxtSyl = NULL;
                preLWd = nxtLWd = NULL;
                prePWd = nxtPWd = NULL;
                prePPh = nxtPPh = NULL;
                preIPh = nxtIPh = NULL;
                sylPos2LWdHead = sylPos2LWdTail = sylPos2PWdHead = sylPos2PWdTail = 0;
                sylPos2PPhHead = sylPos2PPhTail = sylPos2IPhHead = sylPos2IPhTail = sylPos2UttHead = sylPos2UttTail = 0;
                lwdPos2PWdHead = lwdPos2PWdTail = 0;
                pwdPos2PPhHead = pwdPos2PPhTail = pwdPos2IPhHead = pwdPos2IPhTail = pwdPos2UttHead = pwdPos2UttTail = 0;
                pphPos2IPhHead = pphPos2IPhTail = pphPos2UttHead = pphPos2UttTail = 0;
                iphPos2UttHead = iphPos2UttTail = 0;
                iphIntonationType = 0;
            }

            int CSSML2Lab::buildLabInfo(std::vector<CUnitInfo> &inSenInfo, std::vector<CSegInfo> &outSenInfo)
            {
                // skip empty sentence if any
                if (inSenInfo.size() == 0)
                    return ERROR_SUCCESS;

                // get all syllable information
                CSegInfo seg;
                int preBoundaryType = 5;
                int nxtBoundaryType = 5;
                std::vector<CUnitInfo>::const_iterator cit;
                for (cit = inSenInfo.begin(); cit != inSenInfo.end(); ++cit)
                {
                    // unit information
                    seg.pUnit   = cit->pUnit;

                    // syllable information
                    wchar_t initial[10], final[10], tonalfinal[15];
                    int tone;
                    bool isPinyin  = CPinyin::separatePinyin(seg.pUnit->wstrPhoneme.c_str(), initial, final, tone);
                    if (isPinyin)
                    {
                        // Pinyin
                        bool isRetro   = CPinyin::isRetroflex(seg.pUnit->wstrPhoneme.c_str());
                        if (isRetro) final[wcslen(final)-1] = L'\0';
                        str::snwprintf(tonalfinal, 14, L"%ls%ld", final, tone);
                        seg.sylPhoneme = str::wcstombs(seg.pUnit->wstrPhoneme);
                        seg.sylFinal   = str::wcstombs(tonalfinal);
                        seg.sylTone    = tone;
                        seg.sylSegs.clear();
                        if (wcslen(initial) != 0) seg.sylSegs.push_back(str::wcstombs(initial));
                        seg.sylSegs.push_back(seg.sylFinal);
                        if (isRetro) seg.sylSegs.push_back("rr");
                    }
                    else
                    {
                        // Other phonemes (e.g. English)
                        static std::wstring delimiters = L"_/"; // for segmenting phonemes
                        static cmn::CRegexp regvowel   = L"^([a-zA-Z]+)([0-2])$";
                        std::vector<std::wstring> segs;
                        str::tokenize(seg.pUnit->wstrPhoneme, delimiters, segs);
                        // save information
                        seg.sylPhoneme = str::wcstombs(seg.pUnit->wstrPhoneme);
                        seg.sylSegs.clear();
                        for (std::vector<std::wstring>::const_iterator cit = segs.begin(); cit != segs.end(); ++cit)
                        {
                            if (cit->length()==0)
                                continue;
                            std::string pstr = str::wcstombs(L"E"+*cit); // prepend "E"
                            seg.sylSegs.push_back(pstr);
                            if (regvowel.match(cit->c_str()))
                            {
                                seg.sylFinal = pstr;
                                seg.sylTone  = pstr[regvowel.substart(2)+1]-'0'; // add "1" for "E" is added
                            }
                        }
                    }

                    // syllable information
                    seg.sylPos2PWdHead = cit->sylPos2PWdHead + 1;
                    seg.sylPos2PWdTail = cit->sylPos2PWdTail + 1;
                    seg.sylPos2PPhHead = cit->sylPos2PPhHead + 1;
                    seg.sylPos2PPhTail = cit->sylPos2PPhTail + 1;
                    seg.sylPos2IPhHead = cit->sylPos2IPhHead + 1;
                    seg.sylPos2IPhTail = cit->sylPos2IPhTail + 1;

                    // prosodic word information
                    seg.pwdSylNumber   = seg.sylPos2PWdHead + seg.sylPos2PWdTail - 1;
                    seg.pwdPos2PPhHead = cit->pwdPos2PPhHead + 1;
                    seg.pwdPos2PPhTail = cit->pwdPos2PPhTail + 1;
                    seg.pwdPos2IPhHead = cit->pwdPos2IPhHead + 1;
                    seg.pwdPos2IPhTail = cit->pwdPos2IPhTail + 1;

                    // prosodic phrase information
                    seg.pphSylNumber   = seg.sylPos2PPhHead + seg.sylPos2PPhTail - 1;
                    seg.pphPWdNumber   = seg.pwdPos2PPhHead + seg.pwdPos2PPhTail - 1;
                    seg.pphPos2IPhHead = cit->pphPos2IPhHead + 1;
                    seg.pphPos2IPhTail = cit->pphPos2IPhTail + 1;

                    // intonation phrase information
                    seg.iphPWdNumber   = seg.pwdPos2IPhHead + seg.pwdPos2IPhTail - 1;
                    seg.iphPPhNumber   = seg.pphPos2IPhHead + seg.pphPos2IPhTail - 1;
                    seg.iphIntonationType = cit->iphIntonationType;

                    // get syllable boundary type
                    switch (seg.pUnit->boundaryType)
                    {
                    case xml::CSSMLDocument::PROSBOUND_PWORD:   nxtBoundaryType = 1; break;
                    case xml::CSSMLDocument::PROSBOUND_PPHRASE: nxtBoundaryType = 5; break;
                    case xml::CSSMLDocument::PROSBOUND_IPHRASE: nxtBoundaryType = 5; break;
                    case xml::CSSMLDocument::PROSBOUND_SENTENCE:nxtBoundaryType = 5; break;
                    default: nxtBoundaryType = 0; break;
                    }

                    // add segment information
                    seg.sylPreBoundaryType = preBoundaryType;
                    seg.sylNxtBoundaryType = nxtBoundaryType;
                    seg.sylSegNumber       = seg.sylSegs.size();
                    for (int i = 0; i < seg.sylSegNumber; ++i)
                    {
                        seg.segPos2SylHead = i+1;
                        seg.segPos2SylTail = seg.sylSegNumber-i;
                        seg.segPhoneme     = seg.sylSegs[i];
                        outSenInfo.push_back(seg);
                    }

                    // add prosodic boundary
                    // (1) add "sil" for all boundaries except PWD;
                    // (2) the acoustic parameters (e.g. duration) of "sil" is determined by HMM model from the context.
                    switch (seg.pUnit->boundaryType)
                    {
                    case xml::CSSMLDocument::PROSBOUND_PWORD:
                        preBoundaryType = 1;
                        break;
                    case xml::CSSMLDocument::PROSBOUND_PPHRASE:
                    case xml::CSSMLDocument::PROSBOUND_IPHRASE:
                    case xml::CSSMLDocument::PROSBOUND_SENTENCE:
                        preBoundaryType = 5; 
                        if (seg.pUnit->isBreak)
                        {
                            seg.asBreak("sil");
                            seg.pUnit = cit->pUnit;
                            outSenInfo.push_back(seg);
                        }
                        break;
                    default:
                        preBoundaryType = 0;
                        break;
                    }
                }

                // build context information
                CSegInfo *preSeg = NULL, *curSeg = NULL;
                CSegInfo *preSyl = NULL, *curSyl = NULL;
                CSegInfo *prePWd = NULL, *curPWd = NULL;
                CSegInfo *prePPh = NULL, *curPPh = NULL;
                std::vector<CSegInfo*> preSylSegs;
                std::vector<CSegInfo*> prePWdSegs;
                std::vector<CSegInfo*> prePPhSegs;
                std::vector<CSegInfo>::iterator cur;
                for (cur = outSenInfo.begin(); cur != outSenInfo.end(); ++cur)
                {
                    // segment context
                    curSeg = &*cur;
                    curSeg->preSeg = preSeg;
                    if (preSeg != NULL)
                        preSeg->nxtSeg = curSeg;
                    preSeg = curSeg;

                    // syllable context
                    if (curSeg->segPos2SylHead == 1) // the first segment in syllable
                    {
                        // encounter a new syllable
                        curSyl = curSeg;
                        for (std::vector<CSegInfo*>::iterator it = preSylSegs.begin(); it != preSylSegs.end(); ++it)
                        {
                            (*it)->nxtSyl = curSyl;
                        }
                        preSylSegs.clear();
                    }
                    preSylSegs.push_back(curSeg);
                    curSeg->preSyl = preSyl;
                    if (curSeg->segPos2SylTail == 1) // the last segment in syllable
                    {
                        preSyl = curSyl;
                    }

                    // prosodic word context
                    if (curSeg->sylPos2PWdHead == 1 && curSeg->segPos2SylHead == 1) // the first segment in prosodic word
                    {
                        // encounter a new prosodic word
                        curPWd = curSeg;
                        for (std::vector<CSegInfo*>::iterator it = prePWdSegs.begin(); it != prePWdSegs.end(); ++it)
                        {
                            (*it)->nxtPWd = curPWd;
                        }
                        prePWdSegs.clear();
                    }
                    prePWdSegs.push_back(curSeg);
                    curSeg->prePWd = prePWd;
                    if (curSeg->sylPos2PWdTail == 1 && curSeg->segPos2SylTail == 1) // the last segment in prosodic word
                    {
                        prePWd = curPWd;
                    }

                    // prosodic phrase context
                    if (curSeg->sylPos2PPhHead == 1 && curSeg->segPos2SylHead == 1) // the first segment in prosodic phrase
                    {
                        // encounter a new prosodic phrase
                        curPPh = curSeg;
                        for (std::vector<CSegInfo*>::iterator it = prePPhSegs.begin(); it != prePPhSegs.end(); ++it)
                        {
                            (*it)->nxtPPh = curPPh;
                        }
                        prePPhSegs.clear();
                    }
                    prePPhSegs.push_back(curSeg);
                    curSeg->prePPh = prePPh;
                    if (curSeg->sylPos2PPhTail == 1 && curSeg->segPos2SylTail == 1) // the last segment in prosodic phrase
                    {
                        prePPh = curPPh;
                    }
                }

                return ERROR_SUCCESS;
            }


            //////////////////////////////////////////////////////////////////////////
            //
            //  Internal operations for building the context information
            //
            //////////////////////////////////////////////////////////////////////////

            int CSSML2Lab::buildContext(std::vector<CUnitItem> &inSenInfo, std::vector<CUnitInfo> &outSenInfo)
            {
                //===========================================
                // 1) prepare syllable information
                //===========================================

                // get all syllable information
                CUnitInfo unit;
                for (std::vector<CUnitItem>::iterator cit = inSenInfo.begin(); cit != inSenInfo.end(); ++cit)
                {
                    // set syllable information
                    unit.pUnit = &*cit;

                    // add syllable
                    outSenInfo.push_back(unit);
                }

                //===========================================
                // 2) build context information
                //===========================================

                // prepare syllable list
                std::vector<CUnitInfo*> sylList;
                CUnitInfo *cur=NULL, *pre=NULL;
                for (std::vector<CUnitInfo>::iterator cit = outSenInfo.begin(); cit != outSenInfo.end(); ++cit)
                {
                    cur = &*cit;
                    sylList.push_back(cur);
                    // syllable context information
                    if (pre != NULL)
                        pre->nxtSyl = cur;
                    cur->preSyl = pre;
                    pre = cur;
                }

                // get context information
                TPosContext posContext = {0};
                for (size_t i=0; i<sylList.size(); ++i)
                {
                    posContext.idxCurSyllable ++;
                    buildPosContext(posContext, sylList);
                }

                return ERROR_SUCCESS;
            }

            int CSSML2Lab::buildPosContext(TPosContext &posContext, std::vector<CUnitInfo*> &outSenInfo)
            {
                // as utterance boundaries are also intonation phrase boundaries and
                // intonation phrase boundaries are also prosody phrase boundaries,
                // prosody phrase boundaries are also prosody word boundaries,
                // prosody word boundaries are also syllable boundaries,
                // a drop-through strategy is used here.

                // syllable context information
                int idxCurSyl = posContext.idxCurSyllable-1;  // as idxCurSyllable has been incremented by 1
                outSenInfo[idxCurSyl]->sylPos2LWdHead = idxCurSyl - posContext.idxLastLWdBound;
                outSenInfo[idxCurSyl]->sylPos2PWdHead = idxCurSyl - posContext.idxLastPWdBound;
                outSenInfo[idxCurSyl]->sylPos2PPhHead = idxCurSyl - posContext.idxLastPPhBound;
                outSenInfo[idxCurSyl]->sylPos2IPhHead = idxCurSyl - posContext.idxLastIPhBound;
                outSenInfo[idxCurSyl]->sylPos2UttHead = idxCurSyl - posContext.idxLastUttBound;
                int boundType = outSenInfo[idxCurSyl]->pUnit->boundaryType;

                // different prosodic boundary context
                switch (boundType)
                {
                case xml::CSSMLDocument::PROSBOUND_LWORD:
                case xml::CSSMLDocument::PROSBOUND_PWORD:
                case xml::CSSMLDocument::PROSBOUND_PPHRASE:
                case xml::CSSMLDocument::PROSBOUND_IPHRASE:
                case xml::CSSMLDocument::PROSBOUND_SENTENCE:
                    // lexicon word boundary
                    if (posContext.idxCurSyllable > posContext.idxLastLWdBound)
                    {
                        const CUnitInfo *preLWd = (posContext.idxLastLWdBound==0) ? NULL : outSenInfo[posContext.idxLastLWdBound-1];
                        const CUnitInfo *nxtLWd = (posContext.idxCurSyllable>=outSenInfo.size()) ? NULL : outSenInfo[posContext.idxCurSyllable];
                        for (int idx = posContext.idxLastLWdBound; idx < posContext.idxCurSyllable; idx++)
                        {
                            CUnitInfo *sylTgt = outSenInfo[idx];
                            sylTgt->sylPos2LWdTail = posContext.idxCurSyllable - posContext.idxLastLWdBound - sylTgt->sylPos2LWdHead - 1;
                            sylTgt->lwdPos2PWdHead = posContext.idxLWdInPWd;
                            sylTgt->preLWd = preLWd;
                            sylTgt->nxtLWd = nxtLWd;
                        }
                        posContext.idxLWdInPWd ++;
                    }
                    posContext.idxLastLWdBound = posContext.idxCurSyllable;
                    if (boundType == xml::CSSMLDocument::PROSBOUND_LWORD)
                    {
                        // just lexicon word boundary
                        break;
                    }

                    // prosodic word boundary
                    if (posContext.idxCurSyllable > posContext.idxLastPWdBound)
                    {
                        const CUnitInfo *prePWd = (posContext.idxLastPWdBound==0) ? NULL : outSenInfo[posContext.idxLastPWdBound-1];
                        const CUnitInfo *nxtPWd = (posContext.idxCurSyllable>=outSenInfo.size()) ? NULL : outSenInfo[posContext.idxCurSyllable];
                        for (int idx = posContext.idxLastPWdBound; idx < posContext.idxCurSyllable; idx++)
                        {
                            CUnitInfo *sylTgt = outSenInfo[idx];
                            sylTgt->sylPos2PWdTail = posContext.idxCurSyllable - posContext.idxLastPWdBound - sylTgt->sylPos2PWdHead - 1;
                            sylTgt->lwdPos2PWdTail = posContext.idxLWdInPWd - sylTgt->lwdPos2PWdHead - 1;
                            sylTgt->pwdPos2PPhHead = posContext.idxPWdInPPh;
                            sylTgt->pwdPos2IPhHead = posContext.idxPWdInIPh;
                            sylTgt->pwdPos2UttHead = posContext.idxPWdInUtt;
                            sylTgt->prePWd = prePWd;
                            sylTgt->nxtPWd = nxtPWd;
                        }
                        posContext.idxPWdInPPh ++;
                        posContext.idxPWdInIPh ++;
                        posContext.idxPWdInUtt ++;
                        posContext.idxLWdInPWd = 0;
                    }
                    posContext.idxLastPWdBound = posContext.idxCurSyllable;
                    if (boundType == xml::CSSMLDocument::PROSBOUND_PWORD)
                    {
                        // just prosodic word boundary
                        break;
                    }

                    // prosodic phrase boundary
                    if (posContext.idxCurSyllable > posContext.idxLastPPhBound)
                    {
                        const CUnitInfo *prePPh = (posContext.idxLastPPhBound==0) ? NULL : outSenInfo[posContext.idxLastPPhBound-1];
                        const CUnitInfo *nxtPPh = (posContext.idxCurSyllable>=outSenInfo.size()) ? NULL : outSenInfo[posContext.idxCurSyllable];
                        for (int idx = posContext.idxLastPPhBound; idx < posContext.idxCurSyllable; idx++)
                        {
                            CUnitInfo *sylTgt = outSenInfo[idx];
                            sylTgt->sylPos2PPhTail = posContext.idxCurSyllable - posContext.idxLastPPhBound - sylTgt->sylPos2PPhHead - 1;
                            sylTgt->pwdPos2PPhTail = posContext.idxPWdInPPh - sylTgt->pwdPos2PPhHead - 1;
                            sylTgt->pphPos2IPhHead = posContext.idxPPhInIPh;
                            sylTgt->pphPos2UttHead = posContext.idxPPhInUtt;
                            sylTgt->prePPh = prePPh;
                            sylTgt->nxtPPh = nxtPPh;
                        }
                        posContext.idxPPhInIPh ++;
                        posContext.idxPPhInUtt ++;
                        posContext.idxPWdInPPh = 0;
                    }
                    posContext.idxLastPPhBound = posContext.idxCurSyllable;
                    if (boundType == xml::CSSMLDocument::PROSBOUND_PPHRASE)
                    {
                        // just prosodic phrase boundary
                        break;
                    }

                    // intonation phrase boundary
                    if (posContext.idxCurSyllable > posContext.idxLastIPhBound)
                    {
                        const CUnitInfo *preIPh = (posContext.idxLastIPhBound==0) ? NULL : outSenInfo[posContext.idxLastIPhBound-1];
                        const CUnitInfo *nxtIPh = (posContext.idxCurSyllable>=outSenInfo.size()) ? NULL : outSenInfo[posContext.idxCurSyllable];
                        for (int idx = posContext.idxLastIPhBound; idx < posContext.idxCurSyllable; idx++)
                        {
                            CUnitInfo *sylTgt = outSenInfo[idx];
                            sylTgt->sylPos2IPhTail = posContext.idxCurSyllable - posContext.idxLastIPhBound - sylTgt->sylPos2IPhHead - 1;
                            sylTgt->pwdPos2IPhTail = posContext.idxPWdInIPh - sylTgt->pwdPos2IPhHead - 1;
                            sylTgt->pphPos2IPhTail = posContext.idxPPhInIPh - sylTgt->pphPos2IPhHead - 1;
                            sylTgt->iphPos2UttHead = posContext.idxIPhInUtt;
                            sylTgt->iphIntonationType = nxtIPh==NULL ? 1 : 0;
                            sylTgt->preIPh = preIPh;
                            sylTgt->nxtIPh = nxtIPh;
                        }
                        posContext.idxIPhInUtt ++;
                        posContext.idxPWdInIPh = 0;
                        posContext.idxPPhInIPh = 0;
                    }
                    posContext.idxLastIPhBound = posContext.idxCurSyllable;
                    if (boundType == xml::CSSMLDocument::PROSBOUND_IPHRASE)
                    {
                        // just intonation phrase boundary
                        break;
                    }

                    // utterance (sentence) boundary
                    if (posContext.idxCurSyllable > posContext.idxLastUttBound)
                    {
                        for (int idx = posContext.idxLastUttBound; idx < posContext.idxCurSyllable; idx ++)
                        {
                            CUnitInfo *sylTgt = outSenInfo[idx];
                            sylTgt->sylPos2UttTail = posContext.idxCurSyllable - posContext.idxLastUttBound - sylTgt->sylPos2UttHead - 1;
                            sylTgt->pwdPos2UttTail = posContext.idxPWdInUtt - sylTgt->pwdPos2UttHead - 1;
                            sylTgt->pphPos2UttTail = posContext.idxPPhInUtt - sylTgt->pphPos2UttHead - 1;
                            sylTgt->iphPos2UttTail = posContext.idxIPhInUtt - sylTgt->iphPos2UttHead - 1;
                        }
                        posContext.idxPWdInUtt = 0;
                        posContext.idxPPhInUtt = 0;
                        posContext.idxIPhInUtt = 0;
                    }
                    posContext.idxLastUttBound = posContext.idxCurSyllable;
                    break;

                default:
                    // error or no boundary
                    break;
                }

                return ERROR_SUCCESS;
            }

        }//namespace Putonghua
    }
}
