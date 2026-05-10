#include "CWaterData.h"
#include "eWaterArea.h"
#include "CWaterTune.h"
#include  <cstdint>
#include <string.h>
#include <windows.h>
#include "Settings.h"

void CWaterData::switchWater(eWaterArea waterArea)
{
    char* xmlpath = nullptr;
    if (waterArea <= 2)
    {
        *CWaterData::curLoadedArea = waterArea;

        if (waterArea == Cayo_Perico)
        {
            *CWaterData::bd_min_x = Settings::CayoPericoMinX;
            *CWaterData::bd_min_y = Settings::CayoPericoMinY;
            *CWaterData::bd_max_x = Settings::CayoPericoMaxX;
            *CWaterData::bd_max_y = Settings::CayoPericoMaxY;

            CWaterTune::load(const_cast<char*>("common:/data/watertune_HeistIsland.xml"));
            CWaterTune::loadWaterFog();
            char* xmlpath = CWaterTune::GetFirstFile(*CDataFileMgr::sm_Instance, 68); //get water_heistIsland.xml from common
            while (*(uint64_t*)(xmlpath + 0x94) != -1)
            {
                if (strstr(xmlpath, "water_HeistIsland.xml"))
                {
                    CWaterData::unload();
                    CWaterData::load(xmlpath);
                    break;
                }
                CWaterTune::GetNextFile(*CDataFileMgr::sm_Instance); //get next block??
            }
        }
        else if (waterArea == Los_Santos)
        {
            *CWaterData::bd_max_x = Settings::LosSantosMaxX;
            *CWaterData::bd_min_x = Settings::LosSantosMinX;
            *CWaterData::bd_min_y = Settings::LosSantosMinY;
            *CWaterData::bd_max_y = Settings::LosSantosMaxY;

            CWaterTune::load(0);
            CWaterTune::loadWaterFog();
            CWaterData::unload();
            CWaterData::load(0);
        }
        else if (waterArea == Liberty_City)
        {
            *CWaterData::bd_max_x = Settings::LibertyCityMaxX;
            *CWaterData::bd_min_x = Settings::LibertyCityMinX;
            *CWaterData::bd_min_y = Settings::LibertyCityMinY;
            *CWaterData::bd_max_y = Settings::LibertyCityMaxY;

            CWaterTune::load(const_cast<char*>("common:/data/watertune_libertycity.xml"));
            CWaterTune::loadWaterFog();
            CWaterData::unload();
            CWaterData::load(const_cast<char*>("update:/common/data/levels/gta5/water_libertycity.xml"));
        }
    }
}
