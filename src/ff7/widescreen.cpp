/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include "../ff7.h"
#include "../log.h"
#include "defs.h"

void ifrit_wave_effect_sub_595A05() {
    effect100_data fn_data = ff7_externals.effect100_array_data[*ff7_externals.effect100_array_idx];
    int step_value;
    if(!fn_data.field_2) {

    }
    if (fn_data.field_2 >= 5)
    {
        if ( fn_data.field_2 >= 10 )
        {
            fn_data.field_0 = 0xFFFF;
            return;
        }
        step_value = -127 * (fn_data.field_2 - 5) / 5 - 1;
    }
    else
    {
        step_value = 127 * fn_data.field_2 / 5 + 0x80;
    }
    if(fn_data.field_2) {

    }
}

void ifrit_first_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer) {
    if(aspect_ratio == AR_WIDESCREEN) {
        constexpr int viewport_x_fix = -103;
        constexpr int viewport_y_fix = 480 - 332;
        constexpr int viewport_width_1_fix = 340 - 255;
        *(short*)(wave_data_pointer + 8) += + viewport_x_fix;
        *(short*)(wave_data_pointer + 16) += + viewport_x_fix + viewport_width_1_fix * 2;
        *(short*)(wave_data_pointer + 24) += + viewport_x_fix;
        *(short*)(wave_data_pointer + 32) += + viewport_x_fix + viewport_width_1_fix * 2;
    }

    ff7_externals.engine_draw_sub_66A47E(wave_data_pointer);
}

void ifrit_second_third_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer) {
    if(aspect_ratio == AR_WIDESCREEN) {
        constexpr int viewport_x_fix = -103;
        constexpr int viewport_height_fix = 480 - 332;
        constexpr int viewport_width_1_fix = 340 - 255;
        constexpr int viewport_width_2_fix = 87 - 65;
        *(short*)(wave_data_pointer + 8) += + viewport_x_fix + viewport_width_1_fix * 2;
        *(short*)(wave_data_pointer + 16) += + viewport_x_fix + (viewport_width_1_fix + viewport_width_2_fix) * 2;
        *(short*)(wave_data_pointer + 24) += + viewport_x_fix + viewport_width_1_fix * 2;
        *(short*)(wave_data_pointer + 32) += + viewport_x_fix + (viewport_width_1_fix + viewport_width_2_fix) * 2;
    }

    ff7_externals.engine_draw_sub_66A47E(wave_data_pointer);
}

void ifrit_first_wave_effect_widescreen_calculation() {
    int step = aspect_ratio == AR_WIDESCREEN ? 12 : 8;
    int ifrit_wave_effect_data = *ff7_externals.ifrit_wave_effect_pointer;
    for (int i = 0; i <= 20; i++) {
        int offset_y_top = step * i;
        int offset_y_bottom;
        if ( i >= 20 )
            offset_y_bottom = aspect_ratio == AR_WIDESCREEN ? 240 : 166;
        else
            offset_y_bottom = step * i + step;
        *(DWORD*)ifrit_wave_effect_data = 150994944;
        *(DWORD*)(ifrit_wave_effect_data + 4) = 780173567;
        *(WORD*)(ifrit_wave_effect_data + 22) = 268;
        *(byte*)(ifrit_wave_effect_data + 28) = 1;
        *(byte*)(ifrit_wave_effect_data + 12) = 1;
        *(byte*)(ifrit_wave_effect_data + 36) = -2;
        *(byte*)(ifrit_wave_effect_data + 20) = -2;
        *(byte*)(ifrit_wave_effect_data + 21) = offset_y_top;
        *(byte*)(ifrit_wave_effect_data + 13) = offset_y_top;
        *(byte*)(ifrit_wave_effect_data + 37) = offset_y_bottom;
        *(byte*)(ifrit_wave_effect_data + 29) = offset_y_bottom;
        *(WORD*)(ifrit_wave_effect_data + 18) = *ff7_externals.battle_viewport_y + 2 * offset_y_top;
        *(WORD*)(ifrit_wave_effect_data + 10) = *(WORD*)(ifrit_wave_effect_data + 18);
        *(WORD*)(ifrit_wave_effect_data + 34) = *ff7_externals.battle_viewport_y + 2 * offset_y_bottom;
        *(WORD*)(ifrit_wave_effect_data + 26) = *(WORD*)(ifrit_wave_effect_data + 34);
        ifrit_wave_effect_data += 40;
    }
}

void ifrit_second_wave_effect_widescreen_calculation() {
    int step = aspect_ratio == AR_WIDESCREEN ? 12 : 8;
    int ifrit_wave_effect_data = *ff7_externals.ifrit_wave_effect_pointer;
    for (int i = 0; i <= 10; i++)
    {
      int offset_y_top = step * i;
      int offset_y_bottom = step * i + step;
      *(DWORD*)ifrit_wave_effect_data = 150994944;
      *(DWORD*)(ifrit_wave_effect_data + 4) = 780173567;
      *(WORD*)(ifrit_wave_effect_data + 22) = 268;
      *(byte*)(ifrit_wave_effect_data + 28) = 1;
      *(byte*)(ifrit_wave_effect_data + 12) = 1;
      *(byte*)(ifrit_wave_effect_data + 36) = 64;
      *(byte*)(ifrit_wave_effect_data + 20) = 64;
      if (i)
      {
        *(byte*)(ifrit_wave_effect_data + 21) = offset_y_top;
        *(byte*)(ifrit_wave_effect_data + 13) = offset_y_top;
      }
      else
      {
        *(byte*)(ifrit_wave_effect_data + 21) = 1;
        *(byte*)(ifrit_wave_effect_data + 13) = 1;
      }
      *(byte*)(ifrit_wave_effect_data + 37) = offset_y_bottom;
      *(byte*)(ifrit_wave_effect_data + 29) = offset_y_bottom;
      *(WORD*)(ifrit_wave_effect_data + 18) = *ff7_externals.battle_viewport_y + 2 * offset_y_top;
      *(WORD*)(ifrit_wave_effect_data + 10) = *(WORD*)(ifrit_wave_effect_data + 18);
      *(WORD*)(ifrit_wave_effect_data + 34) = *ff7_externals.battle_viewport_y + 2 * offset_y_bottom;
      *(WORD*)(ifrit_wave_effect_data + 26) = *(WORD*)(ifrit_wave_effect_data + 34);
      ifrit_wave_effect_data += 40;
    }
}

void ifrit_third_wave_effect_widescreen_calculation() {
    int step = aspect_ratio == AR_WIDESCREEN ? 12 : 8;
    int ifrit_wave_effect_data = *ff7_externals.ifrit_wave_effect_pointer;
     for (int i = 0; i <= 9; i++)
    {
      int offset_y_top = step * i;
      int offset_y_bottom;
      if ( i >= 9 )
        offset_y_bottom = aspect_ratio == AR_WIDESCREEN ? 120 : 78;
      else
        offset_y_bottom = step * i + step;
      *(DWORD*)ifrit_wave_effect_data = 150994944;
      *(DWORD*)(ifrit_wave_effect_data + 4) = 780173567;
      *(WORD*)(ifrit_wave_effect_data + 22) = 268;
      *(byte*)(ifrit_wave_effect_data + 28) = 1;
      *(byte*)(ifrit_wave_effect_data + 12) = 1;
      *(byte*)(ifrit_wave_effect_data + 36) = 64;
      *(byte*)(ifrit_wave_effect_data + 20) = 64;
      if (i)
      {
        *(byte*)(ifrit_wave_effect_data + 21) = offset_y_top;
        *(byte*)(ifrit_wave_effect_data + 13) = offset_y_top;
      }
      else
      {
        *(byte*)(ifrit_wave_effect_data + 21) = 1;
        *(byte*)(ifrit_wave_effect_data + 13) = 1;
      }
      *(byte*)(ifrit_wave_effect_data + 37) = offset_y_bottom;
      *(byte*)(ifrit_wave_effect_data + 29) = offset_y_bottom;
      *(WORD*)(ifrit_wave_effect_data + 18) = *ff7_externals.battle_viewport_y + 2 * (offset_y_top + (aspect_ratio == AR_WIDESCREEN ? 120 : 88));
      *(WORD*)(ifrit_wave_effect_data + 10) = *(WORD*)(ifrit_wave_effect_data + 18);
      *(WORD*)(ifrit_wave_effect_data + 34) = *ff7_externals.battle_viewport_y + 2 * (offset_y_bottom + (aspect_ratio == AR_WIDESCREEN ? 120 : 88));
      *(WORD*)(ifrit_wave_effect_data + 26) = *(WORD*)(ifrit_wave_effect_data + 34);
      ifrit_wave_effect_data += 40;
    }
}